/*
 * Amazon FreeRTOS HTTPS Client V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @file iot_https_types.h
 * @brief Types of the HTTPS Client library.
 */

#ifndef IOT_HTTPS_TYPES_H_
#define IOT_HTTPS_TYPES_H_

/* The config header is always included first. */
#include "iot_config.h"

/* C standard includes. */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*-----------------------------------------------------------*/

/**
 * @constantspage{https_client,HTTPS Client library}
 */

/**
 * @section https_minimum_user_buffer_sizes HTTPS Client Minimum User Buffer Sizes
 * @brief variables calculating the size of #IotHttpsUserBuffer_t.bufferLen needed for the request, response, and
 * connection.
 *
 * @note These user buffer minumum values may change at any time in future versions, but their names will remain the
 * same.
 */

/**
 * @brief The minimum user buffer size for the HTTP request context and headers.
 *
 * This helps to calculate the size of the buffer needed for #IotHttpsRequestInfo_t.reqUserBuffer.
 *
 * This buffer size is calculated to fit the HTTP request line and the default headers. It does not account for the
 * length of the path in the request line nor does it account for the length of the host name. It also does not account
 * for extra headers that the application may add. These sizes need to be accounted for by the application when
 * assigning a buffer.
 *
 * By the application providing the memory for the internal context, no memory is needed to be allocated internally to
 * the library for the internal context. The application has control over the memory allocation related to the request,
 * response, and connection.
 */
extern const uint32_t requestUserBufferMinimumSize;

/**
 * @brief The minimum user buffer size for the HTTP response context and headers.
 *
 * This helps to calculate the size of the buffer needed for #IotHttpsRequestInfo_t.respUserBuffer.
 *
 * The buffer size is calculated to fit the HTTP response context only. It does not account for the HTTP response status
 * line. It does not account for any HTTP response headers. If the buffer assigned to
 * #IotHttpsRequestInfo_t.respUserBuffer is of this minimum size, then the response status line and the response headers
 * will not be stored. These sizes need to be accounted for by the application when assigning a buffer.
 *
 * If the response status line and response headers cannot fit into #IotHttpsRequestInfo_t.respUserBuffer, then after a
 * call to @ref https_client_function_sendsync, calls to @ref https_client_function_readresponsestatus,
 * @ref https_client_function_readcontentlength, and @ref https_client_function_readheader will return a failure code.
 *
 * By the application providing the memory for the internal context, no memory is needed to be allocated internally to
 * the library for the internal context. The application has control over the memory allocation related to the request,
 * response, and connection.
 */
extern const uint32_t responseUserBufferMinimumSize;

/**
 * @brief The minimum user buffer size for the HTTP connection conext and headers.
 *
 * This helps to calculate the size of the buffer needed for #IotHttpsConnectionInfo_t.userBuffer.
 *
 * The buffer size is calculated to fit the HTTP connection context only. The buffer assigned by the application must be
 * at least this size.
 *
 * By the application providing the memory for the internal context, no memory is needed to be allocated internally to
 * the library for the internal context. The application has control over the memory allocation related to the request,
 * response, and connection.
 */
extern const uint32_t connectionUserBufferMinimumSize;

/**
 * @section https_connection_flags HTTPS Client Connection Flags
 * @brief Flags that modify the behavior of the HTTPS Connection.
 *
 * Flags should be bitwised-ORed with each other to change the behavior of @ref https_client_function_sendasync and
 * @ref https_client_function_sendsync. These flags are set in #IotHttpsConnectionInfo_t.flags.
 *
 * @note The values of flags may change at any time in future versions, but their names will remain the same.
 */

/**
 * @brief Flag for #IotHttpsConnectionInfo_t that disables TLS.
 *
 * Set this bit in #IotHttpsConnectionInfo_t.flags to disable use of TLS when the connection is created. This library
 * creates secure connections by default.
 */
#define IOT_HTTPS_IS_NON_TLS_FLAG           ( 0x00000001 )

/**
 * @brief Flag for #IotHttpsConnectionInfo_t that disables Server Name Indication (SNI).
 *
 * Set this bit  #IotHttpsConnectionInfo_t.flags to disable SNI. SNI is enabled by default in this library. When SNI is
 * enabled  #IotHttpsConnectionInfo_t.pAddress will be used for the server name verification.
 */
#define IOT_HTTPS_DISABLE_SNI               ( 0x00000008 )

/**
 * @section https_initializers HTTP Initializers
 * @brief Provide default values for the data types of the HTTP Client Library.
 *
 * @snippet this define_https_initializers
 *
 * All user-facing data types of the HTTPS Client library should be initialized using one of the following.
 *
 * @warning Failing to initialize an HTTPS Client data type with the appropriate initializer may result in undefined
 * behavior.
 * @note The initializers may change at any time in future versions, but their names will remain the same.
 *
 * <b>Example</b>
 * @code{c}
 * IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;
 * IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;
 * IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
 * IotHttpsUserBuffer_t userBuffer = IOT_HTTPS_USER_BUFFER_INITIALIZER;
 * IotHttpsSyncRequestInfo_t syncInfo = IOT_HTTPS_SYNC_REQUEST_INFO_INITIALIZER;
 * IotHttpsConnectionInfo_t connInfo = IOT_HTTPS_CONNECTION_INFO_INITIALIZER;
 * IotHttpsRequestInfo_t reqInfo = IOT_HTTPS_REQUEST_INFO_INITIALIZER
 * @code
 *
 */
/* @[define_https_initializers] */
/** @brief Initializer for #IotHttpsConnectionHandle_t. */
#define IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER     NULL
/** @brief Initializer for #IotHttpsRequestHandle_t. */
#define IOT_HTTPS_REQUEST_HANDLE_INITIALIZER        NULL
/** @brief Initializer for #IotHttpsResponseHandle_t. */
#define IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER       NULL
/** @brief Initializer for #IotHttpsUserBuffer_t. */
#define IOT_HTTPS_USER_BUFFER_INITIALIZER           { 0 }
/** @brief Initializer for #IotHttpsSyncRequestInfo_t. */
#define IOT_HTTPS_SYNC_REQUEST_INFO_INITIALIZER     { 0 }
/** @brief Initializer for #IotHttpsConnectionInfo_t. */
#define IOT_HTTPS_CONNECTION_INFO_INITIALIZER       { 0 }
/** @brief Initializer for #IotHttpsRequestInfo_t. */
#define IOT_HTTPS_REQUEST_INFO_INITIALIZER          { 0 }
/* @[define_https_initializers] */

/* Amazon FreeRTOS network include for the network types below. */
#include "platform/iot_network.h"

/**
 * @brief Type for the network interface containing the operations to send, receive, connect, and disconnect from
 * the network.
 */
#define IOT_HTTPS_NETWORK_INTERFACE_TYPE                      const IotNetworkInterface_t*

/*-----------------------------------------------------------*/

/**
 * @handles{https_client,HTTPS Client library}
 */

/**
 * @ingroup https_client_datatypes_handles
 * @brief Opaque handle of an HTTP connection.
 *
 * A connection handle is needed to send many requests over a single persistent connection. This handle is valid after
 * a successful call to @ref https_client_function_connect or @ref https_client_sendsync or
 * @ref https_client_function_sendasync. A variable of this type is passed to @ref https_client_function_sendsync,
 * @ref https_client_function_sendasync, and @ref https_client_function_disconnect to identify which connection that
 * function acts on.
 *
 * A call to @ref https_client_disconnect makes a connection handle invalid. Once @ref https_client_function_disconnect
 * returns, the connection handle should no longer be used. The application must call @ref https_client_function_connect
 * again to retrieve a new handle and a new connection.
 *
 * Typical webservers disconnect the client in around 30-60 seconds. The application needs to be aware of this, when
 * taking time between requests in a persistent connection.
 */
typedef struct _httpsConnection *IotHttpsConnectionHandle_t;

/**
 * @ingroup https_client_datatypes_handles
 * @brief Opaque handle of an HTTP request.
 *
 * Having a separate handle for the HTTP request allows the application to re-use a request.
 *
 * This handle is valid after a successful call to @ref https_client_function_initializerequest. A variable of this type
 * is passed to @ref https_client_function_sendasync or @ref https_client_function_sendsync.
 */
typedef struct _httpsRequest *IotHttpsRequestHandle_t;

/**
 * @ingroup https_client_datatypes_handles
 * @brief Opaque handle of an HTTP response.
 *
 * This handle is valid after a successful call to @ref https_client_function_sendsync or
 * @ref https_client_function_sendasync. A variable of this type is passed to
 * @ref https_client_function_readresponsestatus, @ref https_client_function_readcontentlength,
 * @ref https_client_function_readheader, and @ref https_client_function_readresponsebody.
 *
 * When returned from @ref https_client_function_sendsync or @ref https_client_function_sendasync, there is an
 * associated #IotHttpsRequestHandle_t. If the #IotHttpsRequestHandle_t associated with this response is re-initialized
 * with @ref https_client_function_initializerequest, then this response handle is no longer valid.
 */
typedef struct _httpsResponse *IotHttpsResponseHandle_t;

/*-----------------------------------------------------------*/

/**
 * @enums{https_client,HTTPS Client library}
 */

/**
 * @ingroup https_client_datatypes_enums
 * @brief Return codes of [HTTPS Client functions](@ref https_client_functions).
 */
typedef enum IotHttpsReturnCode
{
    IOT_HTTPS_OK = 0,

    /* input/output related */
    IOT_HTTPS_INVALID_PARAMETER = 101,
    IOT_HTTPS_INVALID_PAYLOAD = 102,
    IOT_HTTPS_MESSAGE_TOO_LARGE = 103,
    IOT_HTTPS_OVERFLOW = 104,
    IOT_HTTPS_INSUFFICIENT_MEMORY = 105,
    IOT_HTTPS_QUEUE_FULL = 106,
    IOT_HTTPS_RETRY = 107,
    IOT_HTTPS_NOT_FOUND = 108,
    IOT_HTTPS_MESSAGE_FINISHED = 109,

    /* internal error  */
    IOT_HTTPS_INTERNAL_ERROR = 201,
    IOT_HTTPS_NETWORK_ERROR = 202,
    IOT_HTTPS_CONNECTION_ERROR = 203,
    IOT_HTTPS_STREAM_ERROR = 204,
    IOT_HTTPS_AUTHENTICATION_ERROR = 205,
    IOT_HTTPS_TLS_ERROR = 206,
    IOT_HTTPS_USER_CALLBACK_ERROR = 207,
    IOT_HTTPS_TIMEOUT_ERROR = 208,
    IOT_HTTPS_PROTOCOL_ERROR = 209,
    IOT_HTTPS_SEND_ABORT = 210,
    IOT_HTTPS_RECEIVE_ABORT = 211,
    IOT_HTTPS_ASYNC_SCHEDULING_ERROR = 212,
    IOT_HTTPS_PARSING_ERROR = 213,

    /* other */
    IOT_HTTPS_FATAL = 901,
    IOT_HTTPS_BUSY = 902,
    IOT_HTTPS_TRY_AGAIN = 903,
    IOT_HTTPS_DATA_EXIST = 904,
    IOT_HTTPS_NOT_SUPPORTED = 905
} IotHttpsReturnCode_t;

/**
 * @ingroup https_client_datatypes_enums
 * @brief Types of HTTP methods.
 *
 * The HTTP method is configured in  #IotHttpsRequestInfo_t.method.
 */
typedef enum IotHttpsMethod
{
    IOT_HTTPS_METHOD_GET = 0,   /* Client-to-server method GET */
    IOT_HTTPS_METHOD_HEAD       /* Client-to-server method HEAD */
} IotHttpsMethod_t;

/**
 * @ingroup https_client_datatypes_enums
 * @brief Types of standard HTTP Response status codes.
 *
 * These status codes are taken from RFC 2616. Please see RFC 2616 for a description of each response status.
 */
enum IotHttpsResponseStatus
{
    IOT_HTTPS_STATUS_CONTINUE = 100,
    IOT_HTTPS_STATUS_SWITCHING_PROTOCOLS,
    IOT_HTTPS_STATUS_OK = 200,
    IOT_HTTPS_STATUS_CREATED,
    IOT_HTTPS_STATUS_ACCEPTED,
    IOT_HTTPS_STATUS_NON_AUTHORITIVE_INFORMATION,
    IOT_HTTPS_STATUS_NO_CONTENT,
    IOT_HTTPS_STATUS_RESET_CONTENT,
    IOT_HTTPS_STATUS_PARTIAL_CONTENT,
    IOT_HTTPS_STATUS_MULTIPLE_CHOICES = 300,
    IOT_HTTPS_STATUS_MOVED_PERMANENTLY,
    IOT_HTTPS_STATUS_FOUND,
    IOT_HTTPS_STATUS_SEE_OTHER,
    IOT_HTTPS_STATUS_NOT_MODIFIED,
    IOT_HTTPS_STATUS_USE_PROXY,
    IOT_HTTPS_STATUS_UNUSED,
    IOT_HTTPS_STATUS_TEMPORARY_REDIRECT,
    IOT_HTTPS_STATUS_BAD_REQUEST = 400,
    IOT_HTTPS_STATUS_UNAUTHORIZED,
    IOT_HTTPS_STATUS_PAYMENT_REQUIRED,
    IOT_HTTPS_STATUS_FORBIDDEN,
    IOT_HTTPS_STATUS_NOT_FOUND,
    IOT_HTTPS_STATUS_METHOD_NOT_ALLOWED,
    IOT_HTTPS_STATUS_NOT_ACCEPTABLE,
    IOT_HTTPS_STATUS_PROXY_AUTHENTICATION_REQUIRED,
    IOT_HTTPS_STATUS_REQUEST_TIMEOUT,
    IOT_HTTPS_STATUS_CONFLICT,
    IOT_HTTPS_STATUS_GONE,
    IOT_HTTPS_STATUS_LENGTH_REQUIRED,
    IOT_HTTPS_STATUS_PRECONDITION_FAILED,
    IOT_HTTPS_STATUS_REQUEST_ENTITY_TOO_LARGE,
    IOT_HTTPS_STATUS_REQUEST_URI_TOO_LONG,
    IOT_HTTPS_STATUS_UNSUPPORTED_MEDIA_TYPE,
    IOT_HTTPS_STATUS_REQUEST_RANGE_NOT_SATISFIABLE,
    IOT_HTTPS_STATUS_EXPECTATION_FAILED,
    IOT_HTTPS_STATUS_INTERNAL_SERVER_ERROR = 500,
    IOT_HTTPS_STATUS_NOT_IMPLEMENTED,
    IOT_HTTPS_STATUS_BAD_GATEWAY,
    IOT_HTTPS_STATUS_SERVICE_UNAVAILABLE,
    IOT_HTTPS_STATUS_GATEWAY_TIMEOUT,
    IOT_HTTPS_STATUS_HTTP_VERSION_NOT_SUPPORTED
};

/*-----------------------------------------------------------*/

/**
 * @paramstructs{https_client,HTTPS Client Libray}
 */

/**
 * @ingroup https_client_datatypes_paramstructs
 * @brief User-provided buffer for storing the HTTPS headers and library internal context.
 *
 * @paramfor @ref https_client_function_initializerequest.
 *
 * The user buffer is configured in #IotHttpsConnectionInfo_t.userBuffer and #IotHttpsRequestInfo_t.userBuffer.
 *
 * The minimum size that the buffer must be configured to is indicated by requestUserBufferMinimumSize,
 * responseUserBufferMinimumSize, connectionUserBufferMinimumSize.
 */
typedef struct IotHttpsUserBuffer
{
    uint8_t * pBuffer;   /**< @brief Application provided buffer pointer. */
    uint32_t bufferLen; /**< @brief The length of the application provided buffer. */
} IotHttpsUserBuffer_t;

/**
 * @ingroup https_client_datatypes_paramstructs
 * @brief HTTPS Client synchronous request information.
 *
 * @paramfor @ref https_client_function_initializerequest.
 *
 * This structure is configured in #IotHttpsRequestInfo_t.pSyncInfo.
 *
 * A synchronous request will block until the response is fully received from the network.
 * This structure defines memory locations to store the response body.
 */
typedef struct IotHttpsSyncRequestInfo
{
    /**
     * @brief Pointer to the HTTP request entity body.
     *
     * This is the file or data we want to send. The data is separated from the headers for the flexibility to point to
     * an already established file elsewhere in memory.
     *
     * Set this NULL if there is no request body.
     */
    uint8_t * pReqData;
    uint32_t reqDataLen;                    /**< @brief length of the request data. */

    /**
     * @brief Pointer to the buffer to store the HTTP response entity body.
     *
     * If the length of the buffer provided to store response body is smaller than the amount of body received,
     * then @ref http_client_function_sendsync will return a IOT_HTTPS_INSUFFICIENT_MEMORY error code. Although an error
     * was returned, the first #IotHttpsSyncRequestInfo_t.respDataLen of the response received on the network will
     * still be available in the buffer.
     *
     * If this is set to NULL, then no body from the response will be stored.
     *
     * This data is separated from the header for flexibility to point to an established file location elsewhere in
     * memory.
     */
    uint8_t * pRespData;
    uint32_t respDataLen;                   /**< @brief Length of the response data. */
} IotHttpsSyncRequestInfo_t;


/**
 * @ingroup https_client_datatypes_paramstructs
 * @brief HTTP connection configuration.
 *
 * @paramfor @ref https_client_function_connect or @ref https_client_function_sendsync or
 * @ref https_client_function_sendasync.
 *
 * This parameter is used for explicit connection in @ref https_client_function_connect.
 * For implicit connection it is a parameter in #IotHttpsRequestInfo_t.pConnInfo and passed into
 * @ref https_client_function_sendsync or @ref https_client_function_sendasync.
 *
 * @note The lengths of the strings in this struct should not include the NULL
 * terminator. Strings in this struct do not need to be NULL-terminated.
 */
typedef struct IotHttpsConnectionInfo
{
    /**
     * @brief Remote address that is DNS discoverable.
     *
     * For example: avs-alexa-na.amazon.com.
     */
    const char *pAddress;
    uint32_t addressLen;    /**< @brief remote address length. */

    uint16_t port;          /**< @brief Remote port number */
    /**
     * @brief Flags to configure the HTTPS connection.
     *
     * See @constantspage{https_client,HTTPS Client library} for the available flags.
     */
    uint32_t flags;          /**< @brief Flags to configure the HTTPS connection. */

    /**
     * @brief Timeout waiting for a response from the network in milliseconds.
     *
     * If this is set to zero, it will default to IOT_HTTPS_RESPONSE_WAIT_MS.
     */
    uint32_t timeout;

    const char* pCaCert;     /**< @brief Server trusted certificate store for this connection */
    uint32_t caCertLen;     /**< @brief Server trusted certificate store size */

    const char* pClientCert; /**< @brief Client certificate store for this connection. */
    uint32_t clientCertLen; /**< @brief Client certificate store size. */

    const char* pPrivateKey; /**< @brief Client private key store for this connection. */
    uint32_t privateKeyLen; /**< @brief Client private key store size. */

    /**
     * @brief String of all the ALPN protocols separated by commas needed for this connection.
     *
     * For the protocols needed for the AWS Iot Message broker endpoint please see:
     * https://docs.aws.amazon.com/iot/latest/developerguide/protocols.html
     */
    char* pAlpnProtocols;
    uint32_t alpnProtocolsLen;  /**< @brief ALPN protocol string length. */

    /**
     * @brief User buffer to store the internal connection context.
     *
     * See @ref connectionUserBufferMinimumSize for information about the user buffer configured in
     * #IotHttpsConnectionInfo_t.userBuffer needed to create a valid connection handle.
     */
    IotHttpsUserBuffer_t userBuffer;

    /**
     * @brief The IOT network abstraction interface.
     *
     * This contains the interface to connect, disconnect, send data, and receive data from the network.
     *
     * In Amazon FreeRTOS this should be of the type IotNetworkInterface_t.
     */
    IOT_HTTPS_NETWORK_INTERFACE_TYPE pNetworkInterface;
} IotHttpsConnectionInfo_t;

/**
 * @ingroup https_client_datatypes_paramstructs
 * @brief HTTP request configuration.
 *
 * @paramfor @ref https_client_function_initializerequest.
 *
 * This parameter is used to configure the request in https_client_function_initializerequest.
 *
 * @note The lengths of the strings in this struct should not include the NULL
 * terminator. Strings in this struct do not need to be NULL-terminated.
 */
typedef struct IotHttpsRequestInfo
{
    /* The path and the method are used to generate the first request line in the HTTP request message. See
       @ref https_client_function_initializerequest for more information. */
    const char *pPath;                  /**< @brief URI path, e.g., "/v20160207/directives?query" */
    uint32_t pathLen;                   /**< @brief URI path length */
    IotHttpsMethod_t method;            /**< @brief HTTP method. See #IotHttpsMethod_t for the list of available methods. */

    /**
     * @brief Host address this request is intended for, e.g., "awsamazon.com".
     *
     * This is the same as the address in #IotHttpsConnectionInfo_t.pAddress. This is here in the request structure to
     * automatically generate the "Host" header field in the header buffer space configured in
     * #IotHttpsRequestInfo_t.reqUserBuffer. See @ref https_client_function_initializerequest for more informaiton.
     */
    const char * pHost;
    uint32_t hostLen;                   /**< @brief Host address length. */

    /**
     * @brief Flag denoting if the connection should be non-persistent.
     *
     * If this flag is set to false, then the connection is persistent. When the connection is persistent, the HTTP
     * header "Connection: keep-alive" is automatically added to the headers to send to the server. This header
     * asks the server to not close the connection after sending the response.
     *
     * If this flag is set to true, then the connection is non-persistent. When the connection is non-persistent, then
     * HTTP header "Connection: close" is automatically added to the headers to send to the server. This header asks
     * the server to close the connection after sending the response.
     *
     * Please see https://tools.ietf.org/html/rfc2616#section-8.1.1 for more details.
     */
    bool isNonPersistent;

    /**
     * @brief Application owned buffer for storing the request headers and internal request context.
     *
     * If this is set to NULL, memory will be allocated internally for the request context and request headers.
     *
     * For an asychronous request, if the application owns the memory for this buffer, then it must not be modified,
     * freed, or reused until the the #IotHttpCallbacks_t.responseCompleteCallback is invoked.
     *
     * Please see #IotHttpsUserBuffer_t for more information.
     */
    IotHttpsUserBuffer_t reqUserBuffer;

    /**
     * The application owned buffer for storing the response headers and internal response context.
     *
     * If this is set to NULL, memory will be allocated internally for the response context and response headers.
     *
     * For an asychronous request, if the application owns the memory for this buffer, then it must not be modified,
     * freed, or reused until the the #IotHttpCallbacks_t.responseCompleteCallback is invoked.
     *
     * Please see #IotHttpsUserBuffer_t for more information.
     */
    IotHttpsUserBuffer_t respUserBuffer;

    IotHttpsSyncRequestInfo_t *pSyncInfo; /**< @brief Information specifically for synchronous requests. There will be future support of asynchronous requests. */

    /**
     * @brief HTTPS Client connection configuration.
     *
     * This is used for an implicit connection in @ref https_client_function_sendsync or
     * @ref https_client_function_sendasync.
     */
    IotHttpsConnectionInfo_t *pConnInfo;
} IotHttpsRequestInfo_t;

#endif