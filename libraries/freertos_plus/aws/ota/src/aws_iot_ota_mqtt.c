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

#define OTA_MAX_PUBLISH_RETRIES            3                /* Max number of publish retries */
#define OTA_PUBLISH_RETRY_DELAY_MS         1000UL           /* Delay between publish retries */
#define OTA_SUBSCRIBE_WAIT_MS              30000UL
#define OTA_UNSUBSCRIBE_WAIT_MS            1000UL
#define OTA_PUBLISH_WAIT_MS                10000UL

/* Publish a message using the platforms PubSub mechanism. */

static IotMqttError_t _OTAPublishMessage( void * pvClient,
                                            const char * const pacTopic,
                                            uint16_t usTopicLen,
                                            char * pcMsg,
                                            uint32_t ulMsgSize,
                                            IotMqttQos_t eQOS );

/* Publish a message to the specified client/topic at the given QOS. */

static IotMqttError_t _OTAPublishMessage( void * const pvClient,
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
    xPublishParams.retain = false;

    eResult = IotMqtt_TimedPublish( pvClient, &xPublishParams, 0, OTA_PUBLISH_WAIT_MS );

    return eResult;
}
