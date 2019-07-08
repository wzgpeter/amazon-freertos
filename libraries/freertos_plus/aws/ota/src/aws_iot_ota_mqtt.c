/*
 * Amazon FreeRTOS OTA V1.0.2
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* MQTT includes. */
#include "iot_mqtt.h"

/* OTA agent includes. */
#include "aws_iot_ota_agent.h"

/* Internal header file for shared definitions. */
#include "aws_ota_agent_internal.h"

#include "aws_application_version.h"

#define OTA_MAX_PUBLISH_RETRIES            3                /* Max number of publish retries */
#define OTA_PUBLISH_RETRY_DELAY_MS         1000UL           /* Delay between publish retries */
#define OTA_SUBSCRIBE_WAIT_MS              30000UL
#define OTA_UNSUBSCRIBE_WAIT_MS            1000UL
#define OTA_PUBLISH_WAIT_MS                10000UL

/*lint -e830 -e9003 Keep these in one location for easy discovery should they change in the future. */
/* Topic strings used by the OTA process. */
/* These first few are topic extensions to the dynamic base topic that includes the Thing name. */
static const char pcOTA_JobsGetNextAccepted_TopicTemplate[] = "$aws/things/%s/jobs/$next/get/accepted";
static const char pcOTA_JobsNotifyNext_TopicTemplate[] = "$aws/things/%s/jobs/notify-next";
static const char pcOTA_JobsGetNext_TopicTemplate[] = "$aws/things/%s/jobs/$next/get";
static const char pcOTA_JobStatus_TopicTemplate[] = "$aws/things/%s/jobs/%s/update";
static const char pcOTA_StreamData_TopicTemplate[] = "$aws/things/%s/streams/%s/data/cbor";
static const char pcOTA_GetStream_TopicTemplate[] = "$aws/things/%s/streams/%s/get/cbor";
static const char pcOTA_GetNextJob_MsgTemplate[] = "{\"clientToken\":\"%u:%s\"}";
static const char pcOTA_JobStatus_StatusTemplate[] = "{\"status\":\"%s\",\"statusDetails\":{";
static const char pcOTA_JobStatus_ReceiveDetailsTemplate[] = "\"%s\":\"%u/%u\"}}";
static const char pcOTA_JobStatus_SelfTestDetailsTemplate[] = "\"%s\":\"%s\",\"" OTA_JSON_UPDATED_BY_KEY "\":\"0x%x\"}}";
static const char pcOTA_JobStatus_ReasonStrTemplate[] = "\"reason\":\"%s: 0x%08x\"}}";
static const char pcOTA_JobStatus_SucceededStrTemplate[] = "\"reason\":\"%s v%u.%u.%u\"}}";
static const char pcOTA_JobStatus_ReasonValTemplate[] = "\"reason\":\"0x%08x: 0x%08x\"}}";
static const char pcOTA_String_Receive[] = "receive";

/* Update the job status topic with our progress of the OTA transfer. */

static void prvUpdateJobStatus_mqtt(OTA_FileContext_t* C,
	                                OTA_JobStatus_t eStatus,
	                                int32_t lReason,
	                                int32_t lSubReason);

/* Called when a MQTT message is received on an OTA agent topic of interest. */

static void prvOTAPublishCallback(void* pvCallbackContext,
	                              IotMqttCallbackParam_t* const pxPublishData);

/* Subscribe to the jobs notification topic (i.e. New file version available). */

static BaseType_t prvSubscribeToJobNotificationTopics(void);

/* Subscribe to the firmware update receive topic. */

static BaseType_t prvSubscribeToDataStream(OTA_FileContext_t* C);

/* UnSubscribe from the firmware update receive topic. */

static BaseType_t prvUnSubscribeFromDataStream(OTA_FileContext_t* C);

/* UnSubscribe from the jobs notification topic. */

static void prvUnSubscribeFromJobNotificationTopic(void);

/* Publish a message using the platforms PubSub mechanism. */

static IotMqttError_t prvPublishMessage( void * pvClient,
                                         const char * const pacTopic,
                                         uint16_t usTopicLen,
                                         char * pcMsg,
                                         uint32_t ulMsgSize,
                                         IotMqttQos_t eQOS );

/* Update the job status on the service side with progress or completion info. */

static void prvUpdateJobStatus_mqtt(OTA_FileContext_t* C,
	                                OTA_JobStatus_t eStatus,
	                                int32_t lReason,
	                                int32_t lSubReason)
{
	DEFINE_OTA_METHOD_NAME("prvUpdateJobStatus_mqtt");

	uint32_t ulTopicLen, ulNumBlocks, ulReceived, ulMsgSize;
	IotMqttError_t eResult;
	IotMqttQos_t eQOS;
	char pcMsg[OTA_STATUS_MSG_MAX_SIZE];
	char pcTopicBuffer[OTA_MAX_TOPIC_LEN];

	/* All job state transitions except streaming progress use QOS 1 since it is required to have status in the job document. */
	eQOS = IOT_MQTT_QOS_1;

	/* A message size of zero means don't publish anything. */
	ulMsgSize = 0UL;

	if (eStatus == eJobStatus_InProgress)
	{
		if (lReason == (int32_t)eJobReason_Receiving)
		{
			if (C != NULL)
			{
				ulNumBlocks = (C->ulFileSize + (OTA_FILE_BLOCK_SIZE - 1U)) >> otaconfigLOG2_FILE_BLOCK_SIZE;
				ulReceived = ulNumBlocks - C->ulBlocksRemaining;

				if ((ulReceived % OTA_UPDATE_STATUS_FREQUENCY) == 0U) /* Output a status update once in a while. */
				{
					/* Downgrade Progress updates to QOS 0 to avoid overloading MQTT buffers during active streaming. */
					eQOS = IOT_MQTT_QOS_0;
					ulMsgSize = (uint32_t)snprintf(pcMsg, /*lint -e586 Intentionally using snprintf. */
						sizeof(pcMsg),
						pcOTA_JobStatus_StatusTemplate,
						pcOTA_JobStatus_Strings[eStatus]);
					ulMsgSize += (uint32_t)snprintf(&pcMsg[ulMsgSize], /*lint -e586 Intentionally using snprintf. */
						sizeof(pcMsg) - ulMsgSize,
						pcOTA_JobStatus_ReceiveDetailsTemplate,
						pcOTA_String_Receive,
						ulReceived,
						ulNumBlocks);
				}
				else
				{ /* Don't send a status update yet. */
					ulMsgSize = 0UL;
				}
			}
			else
			{
				/* Can't send a status update without data from the OTA context. Some calls intentionally
				 * don't use a context structure but never with this reason code so log this error. */
				OTA_LOG_L1("[%s] Error: null context pointer!\r\n", OTA_METHOD_NAME);
				ulMsgSize = 0UL;
			}
		}
		else
		{
			/* We're no longer receiving but we're still In Progress so we are implicitly in the Self
			 * Test phase. Prepare to update the job status with the self_test phase (ready or active). */
			ulMsgSize = (uint32_t)snprintf(pcMsg, /*lint -e586 Intentionally using snprintf. */
				sizeof(pcMsg),
				pcOTA_JobStatus_StatusTemplate,
				pcOTA_JobStatus_Strings[eStatus]);
			ulMsgSize += (uint32_t)snprintf(&pcMsg[ulMsgSize], /*lint -e586 Intentionally using snprintf. */
				sizeof(pcMsg) - ulMsgSize,
				pcOTA_JobStatus_SelfTestDetailsTemplate,
				pcOTA_JSON_SelfTestKey,
				pcOTA_JobReason_Strings[lReason],
				xAppFirmwareVersion.u.ulVersion32);
		}
	}
	else
	{
		if (eStatus < eNumJobStatusMappings)
		{
			/* Status updates that are NOT "IN PROGRESS" or "SUCCEEDED" map status and reason codes
			 * to a string plus a sub-reason code except for FailedWithVal status. FailedWithVal uses
			 * a numeric OTA error code and sub-reason code to cover the case where there may be too
			 * many description strings to reasonably include in the code.
			 */
			ulMsgSize = (uint32_t)snprintf(pcMsg, /*lint -e586 Intentionally using snprintf. */
				sizeof(pcMsg),
				pcOTA_JobStatus_StatusTemplate,
				pcOTA_JobStatus_Strings[eStatus]);

			if (eStatus == eJobStatus_FailedWithVal)
			{
				ulMsgSize += (uint32_t)snprintf(&pcMsg[ulMsgSize], /*lint -e586 Intentionally using snprintf. */
					sizeof(pcMsg) - ulMsgSize,
					pcOTA_JobStatus_ReasonValTemplate,
					lReason,
					lSubReason);
			}

			/* If the status update is for "SUCCEEDED," we are identifying the version of firmware
			 * that has been accepted. This makes it easy to find the version associated with each
			 * device (aka Thing) when examining the OTA jobs on the service side via the CLI or
			 * possibly with some console tool.
			 */
			else if (eStatus == eJobStatus_Succeeded)
			{
				AppVersion32_t xNewVersion;

				xNewVersion.u.lVersion32 = lSubReason;
				ulMsgSize += (uint32_t)snprintf(&pcMsg[ulMsgSize], /*lint -e586 Intentionally using snprintf. */
					sizeof(pcMsg) - ulMsgSize,
					pcOTA_JobStatus_SucceededStrTemplate,
					pcOTA_JobReason_Strings[lReason],
					xNewVersion.u.x.ucMajor,
					xNewVersion.u.x.ucMinor,
					xNewVersion.u.x.usBuild);
			}
			else
			{
				ulMsgSize += (uint32_t)snprintf(&pcMsg[ulMsgSize], /*lint -e586 Intentionally using snprintf. */
					sizeof(pcMsg) - ulMsgSize,
					pcOTA_JobStatus_ReasonStrTemplate,
					pcOTA_JobReason_Strings[lReason],
					lSubReason);
			}
		}
		else
		{ /* Unknown status code. Just ignore it. */
			ulMsgSize = 0UL;
		}
	}

	if (ulMsgSize > 0UL)
	{
		/* Try to build the dynamic job status topic . */
		ulTopicLen = (uint32_t)snprintf(pcTopicBuffer, /*lint -e586 Intentionally using snprintf. */
			sizeof(pcTopicBuffer),
			pcOTA_JobStatus_TopicTemplate,
			xOTA_Agent.pcThingName,
			xOTA_Agent.pcOTA_Singleton_ActiveJobName);

		/* If the topic name was built, try to publish the status message to it. */
		if ((ulTopicLen > 0UL) && (ulTopicLen < sizeof(pcTopicBuffer)))
		{
			OTA_LOG_L1("[%s] Msg: %s\r\n", OTA_METHOD_NAME, pcMsg);
			eResult = prvPublishMessage(
				xOTA_Agent.pvPubSubClient,
				pcTopicBuffer,
				(uint16_t)ulTopicLen,
				&pcMsg[0],
				ulMsgSize,
				eQOS);

			if (eResult != IOT_MQTT_SUCCESS)
			{
				OTA_LOG_L1("[%s] Failed: %s\r\n", OTA_METHOD_NAME, pcTopicBuffer);
			}
			else
			{
				OTA_LOG_L1("[%s] '%s' to %s\r\n", OTA_METHOD_NAME, pcOTA_JobStatus_Strings[eStatus], pcTopicBuffer);
			}
		}
		else
		{
			OTA_LOG_L1("[%s] Failed to build job status topic!\r\n", OTA_METHOD_NAME);
		}
	}
	else
	{
		/* Just ignore the zero length message. */
	}
}

/* This function is called whenever we receive a MQTT publish message on one of our OTA topics. */

static void prvOTAPublishCallback(void* pvCallbackContext,
	                              IotMqttCallbackParam_t* const pxPublishData)
{
	DEFINE_OTA_METHOD_NAME_L2("prvOTAPublishCallback");

	BaseType_t xReturn;
	OTA_PubMsg_t* pxMsg;

	if (pxPublishData->u.message.info.payloadLength > OTA_DATA_BLOCK_SIZE)
	{
		OTA_LOG_L1("Error: buffers are too small %d to contains the payload %d.\r\n", OTA_DATA_BLOCK_SIZE, pxPublishData->u.message.info.payloadLength);
		return;
	}
	/* If we're running the OTA task, send publish messages to it for processing. */
	if (xOTA_Agent.xOTA_EventFlags != NULL)
	{
		xOTA_Agent.xStatistics.ulOTA_PacketsReceived++;

		/* Lock up a buffer to copy publish data. */
		pxMsg = prvOTAPubMessageGet();

		if (pxMsg != NULL)
		{
			pxMsg->lMsgType = (int32_t)pvCallbackContext; /*lint !e923 The context variable is actually the message type. */
			pxMsg->pxPubData.ulDataLength = pxPublishData->u.message.info.payloadLength;
			if ((int32_t)pvCallbackContext == eOTA_PubMsgType_Stream) {
				OTA_LOG_L2("[%s] Stream Received.\r\n", OTA_METHOD_NAME);
			}


			memcpy(pxMsg->pxPubData.vData, pxPublishData->u.message.info.pPayload, pxMsg->pxPubData.ulDataLength);
			xReturn = xQueueSendToBack(xOTA_Agent.xOTA_MsgQ, &pxMsg, (TickType_t)0);
			if (xReturn == pdPASS)
			{
				xOTA_Agent.xStatistics.ulOTA_PacketsQueued++;
				(void)xEventGroupSetBits(xOTA_Agent.xOTA_EventFlags, OTA_EVT_MASK_MSG_READY);
				/* Take ownership of the MQTT buffer. */
			}
			else
			{
				OTA_LOG_L1("Error: Could not push message to queue.\r\n");
				/* Free up locked buffer. */
				prvOTAPubMessageFree(pxMsg);
				xOTA_Agent.xStatistics.ulOTA_PacketsDropped++;
			}
		}
		else
		{
			xOTA_Agent.xStatistics.ulOTA_PacketsDropped++;
			OTA_LOG_L1("Error: Could not get a free buffer to copy callback data.\r\n");
		}
	}
	else
	{
		/* This doesn't normally occur unless we're subscribed to an OTA topic when
		 * the OTA agent is not initialized. Just drop the message by not taking
		 * ownership since we don't know if we'll ever be able to process it. */
		OTA_LOG_L2("[%s] Warning: Received MQTT message but agent isn't ready.\r\n", OTA_METHOD_NAME);
		xOTA_Agent.xStatistics.ulOTA_PacketsDropped++;
	}
}


/* Subscribe to the OTA job notification topics. */

static BaseType_t prvSubscribeToJobNotificationTopics(void)
{
	DEFINE_OTA_METHOD_NAME("prvSubscribeToJobNotificationTopics");

	BaseType_t xResult = pdFALSE;
	char pcJobTopic[OTA_MAX_TOPIC_LEN];
	IotMqttSubscription_t xJobsSubscription;

	/* Clear subscription struct and set common parameters for job topics used by OTA. */
	memset(&xJobsSubscription, 0, sizeof(xJobsSubscription));
	xJobsSubscription.qos = IOT_MQTT_QOS_1;
	xJobsSubscription.pTopicFilter = (const char*)pcJobTopic;         /* Point to local string storage. Built below. */
	xJobsSubscription.callback.pCallbackContext = (void*)eOTA_PubMsgType_Job;      /*lint !e923 The publish callback context is implementing data hiding with a void* type.*/
	xJobsSubscription.callback.function = prvOTAPublishCallback;
	xJobsSubscription.topicFilterLength = (uint16_t)snprintf(pcJobTopic, /*lint -e586 Intentionally using snprintf. */
		sizeof(pcJobTopic),
		pcOTA_JobsGetNextAccepted_TopicTemplate,
		xOTA_Agent.pcThingName);

	if ((xJobsSubscription.topicFilterLength > 0U) && (xJobsSubscription.topicFilterLength < sizeof(pcJobTopic)))
	{
		/* Subscribe to the first of two jobs topics. */
		if (IotMqtt_TimedSubscribe(xOTA_Agent.pvPubSubClient,
			&xJobsSubscription,
			1, /* Subscriptions count */
			0, /* flags */
			OTA_SUBSCRIBE_WAIT_MS) != IOT_MQTT_SUCCESS)
		{
			OTA_LOG_L1("[%s] Failed: %s\n\r", OTA_METHOD_NAME, xJobsSubscription.pTopicFilter);
		}
		else
		{
			OTA_LOG_L1("[%s] OK: %s\n\r", OTA_METHOD_NAME, xJobsSubscription.pTopicFilter);
			xJobsSubscription.topicFilterLength = (uint16_t)snprintf(pcJobTopic, /*lint -e586 Intentionally using snprintf. */
				sizeof(pcJobTopic),
				pcOTA_JobsNotifyNext_TopicTemplate,
				xOTA_Agent.pcThingName);

			if ((xJobsSubscription.topicFilterLength > 0U) && (xJobsSubscription.topicFilterLength < sizeof(pcJobTopic)))
			{
				/* Subscribe to the second of two jobs topics. */
				if (IotMqtt_TimedSubscribe(xOTA_Agent.pvPubSubClient,
					&xJobsSubscription,
					1, /* Subscriptions count */
					0, /* flags */
					OTA_SUBSCRIBE_WAIT_MS) != IOT_MQTT_SUCCESS)
				{
					OTA_LOG_L1("[%s] Failed: %s\n\r", OTA_METHOD_NAME, xJobsSubscription.pTopicFilter);
				}
				else
				{
					OTA_LOG_L1("[%s] OK: %s\n\r", OTA_METHOD_NAME, xJobsSubscription.pTopicFilter);
					xResult = pdTRUE;
				}
			}
		}
	}

	return xResult;
}

/* Subscribe to the OTA data stream topic. */

static BaseType_t prvSubscribeToDataStream(OTA_FileContext_t* C)
{
	DEFINE_OTA_METHOD_NAME("prvSubscribeToDataStream");

	BaseType_t xResult = pdFALSE;
	char pcOTA_RxStreamTopic[OTA_MAX_TOPIC_LEN];
	IotMqttSubscription_t xOTAUpdateDataSubscription;

	memset(&xOTAUpdateDataSubscription, 0, sizeof(xOTAUpdateDataSubscription));
	xOTAUpdateDataSubscription.qos = IOT_MQTT_QOS_0;
	xOTAUpdateDataSubscription.pTopicFilter = (const char*)pcOTA_RxStreamTopic;
	xOTAUpdateDataSubscription.callback.pCallbackContext = (void*)eOTA_PubMsgType_Stream;  /*lint !e923 The publish callback context is implementing data hiding with a void* type.*/
	xOTAUpdateDataSubscription.callback.function = prvOTAPublishCallback;
	xOTAUpdateDataSubscription.topicFilterLength = (uint16_t)snprintf(pcOTA_RxStreamTopic, /*lint -e586 Intentionally using snprintf. */
		sizeof(pcOTA_RxStreamTopic),
		pcOTA_StreamData_TopicTemplate,
		xOTA_Agent.pcThingName,
		(const char*)C->pucStreamName);

	if ((xOTAUpdateDataSubscription.topicFilterLength > 0U) && (xOTAUpdateDataSubscription.topicFilterLength < sizeof(pcOTA_RxStreamTopic)))
	{
		if (IotMqtt_TimedSubscribe(xOTA_Agent.pvPubSubClient,
			&xOTAUpdateDataSubscription,
			1, /* Subscriptions count */
			0, /* flags */
			OTA_SUBSCRIBE_WAIT_MS) != IOT_MQTT_SUCCESS)
		{
			OTA_LOG_L1("[%s] Failed: %s\n\r", OTA_METHOD_NAME, xOTAUpdateDataSubscription.pTopicFilter);
		}
		else
		{
			OTA_LOG_L1("[%s] OK: %s\n\r", OTA_METHOD_NAME, xOTAUpdateDataSubscription.pTopicFilter);
			xResult = pdTRUE;
		}
	}
	else
	{
		OTA_LOG_L1("[%s] Failed to build stream topic.\n\r", OTA_METHOD_NAME);
	}

	return xResult;
}

/* UnSubscribe from the OTA data stream topic. */

static BaseType_t prvUnSubscribeFromDataStream( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvUnSubscribeFromDataStream" );

    IotMqttSubscription_t xUnsub;

	BaseType_t xResult = pdFALSE;
    char pcOTA_RxStreamTopic[ OTA_MAX_TOPIC_LEN ];

	xUnsub.qos = IOT_MQTT_QOS_0;

    if( C != NULL )
    {
        /* Try to build the dynamic data stream topic and un-subscribe from it. */

		xUnsub.topicFilterLength = ( uint16_t ) snprintf( pcOTA_RxStreamTopic, /*lint -e586 Intentionally using snprintf. */
                                                           sizeof( pcOTA_RxStreamTopic ),
                                                           pcOTA_StreamData_TopicTemplate,
                                                           xOTA_Agent.pcThingName,
                                                           ( const char * ) C->pucStreamName );

        if( (xUnsub.topicFilterLength > 0U ) && (xUnsub.topicFilterLength < sizeof( pcOTA_RxStreamTopic ) ) )
        {
			xUnsub.pTopicFilter = ( const char * ) pcOTA_RxStreamTopic;

            if( IotMqtt_TimedUnsubscribe( xOTA_Agent.pvPubSubClient,
                                          &xUnsub,
                                          1, /* Subscriptions count */
                                          0, /* flags */
									      OTA_UNSUBSCRIBE_WAIT_MS ) != IOT_MQTT_SUCCESS )
            {
                OTA_LOG_L1( "[%s] Failed: %s\n\r", OTA_METHOD_NAME, pcOTA_RxStreamTopic );
            }
            else
            {
                OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, pcOTA_RxStreamTopic );
                xResult = pdTRUE;
            }
        }
        else
        {
            OTA_LOG_L1( "[%s] Failed to build stream topic.\n\r", OTA_METHOD_NAME );
        }
    }

    return xResult;
}


/* UnSubscribe from the OTA job notification topics. */

static void prvUnSubscribeFromJobNotificationTopic( void )
{
    DEFINE_OTA_METHOD_NAME( "prvUnSubscribeFromJobNotificationTopic" );

    IotMqttSubscription_t xUnsub;
    IotMqttOperation_t unsubscribeOperation[ 2 ] = { NULL };
    char pcJobTopic[ OTA_MAX_TOPIC_LEN ];

    /* Try to unsubscribe from the first of two job topics. */
	xUnsub.qos = IOT_MQTT_QOS_0;
	xUnsub.pTopicFilter = ( const char * ) pcJobTopic;            /* Point to local string storage. Built below. */
	xUnsub.topicFilterLength = ( uint16_t ) snprintf( pcJobTopic, /*lint -e586 Intentionally using snprintf. */
                                                      sizeof( pcJobTopic ),
                                                      pcOTA_JobsNotifyNext_TopicTemplate,
                                                      xOTA_Agent.pcThingName );

    if( (xUnsub.topicFilterLength > 0U ) && (xUnsub.topicFilterLength < sizeof( pcJobTopic ) ) )
    {
        if( IotMqtt_Unsubscribe( xOTA_Agent.pvPubSubClient,
                                 &xUnsub,
                                 1, /* Subscriptions count */
                                 IOT_MQTT_FLAG_WAITABLE, /* flags */
								 NULL,
                                 &( unsubscribeOperation[ 0]) ) != IOT_MQTT_STATUS_PENDING )
        {
            OTA_LOG_L1( "[%s] FAIL: %s\n\r", OTA_METHOD_NAME, xUnsub.pTopicFilter );
        }
        else
        {
            OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, xUnsub.pTopicFilter );
        }
    }

    /* Try to unsubscribe from the second of two job topics. */
	xUnsub.topicFilterLength = ( uint16_t ) snprintf( pcJobTopic, /*lint -e586 Intentionally using snprintf. */
                                                       sizeof( pcJobTopic ),
                                                       pcOTA_JobsGetNextAccepted_TopicTemplate,
                                                       xOTA_Agent.pcThingName );

    if( (xUnsub.topicFilterLength > 0U ) && (xUnsub.topicFilterLength < sizeof( pcJobTopic ) ) )
    {
        if( IotMqtt_Unsubscribe( xOTA_Agent.pvPubSubClient,
                                 &xUnsub,
                                 1, /* Subscriptions count */
                                 IOT_MQTT_FLAG_WAITABLE, /* flags */
								 NULL,
                                 &( unsubscribeOperation[ 1]) ) != IOT_MQTT_STATUS_PENDING )
        {
            OTA_LOG_L1( "[%s] FAIL: %s\n\r", OTA_METHOD_NAME, xUnsub.pTopicFilter );
        }
        else
        {
            OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, xUnsub.pTopicFilter );
        }
    }

    if( unsubscribeOperation[0] != NULL)
    {
    	IotMqtt_Wait( unsubscribeOperation[ 0 ], OTA_UNSUBSCRIBE_WAIT_MS);
    }
    if( unsubscribeOperation[1] != NULL)
    {
    	IotMqtt_Wait( unsubscribeOperation[ 1 ], OTA_UNSUBSCRIBE_WAIT_MS);
    }
}


/* Publish a message to the specified client/topic at the given QOS. */

static IotMqttError_t prvPublishMessage( void * const pvClient,
                                            const char * const pacTopic,
                                            uint16_t usTopicLen,
                                            char * pcMsg,
                                            uint32_t ulMsgSize,
                                            IotMqttQos_t eQOS )
{
    IotMqttError_t eResult;
    IotMqttPublishInfo_t xPublishParams;

    xPublishParams.pTopicName = ( const char * ) pacTopic;
    xPublishParams.topicNameLength = usTopicLen;
    xPublishParams.qos = eQOS;
    xPublishParams.pPayload = pcMsg;
    xPublishParams.payloadLength = ulMsgSize;
    xPublishParams.retryLimit = OTA_MAX_PUBLISH_RETRIES;
    xPublishParams.retryMs = OTA_PUBLISH_RETRY_DELAY_MS;
	xPublishParams.retain = pdFALSE;

    eResult = IotMqtt_TimedPublish( pvClient, &xPublishParams, 0, OTA_PUBLISH_WAIT_MS );

    return eResult;
}
