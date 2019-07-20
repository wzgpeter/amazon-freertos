/*
 * Amazon FreeRTOS V201906.00 Major
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

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

/*
 * @brief PEM-encoded client certificate.
 *
 * @todo If you are running one of the Amazon FreeRTOS demo projects, set this
 * to the certificate that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIUE+CmVW3LxaWY4Dvwb1RjgBFaFQUwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDcxODE5MjQ1\n"\
"MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMtqpuPJEVeHB+N6YaLi\n"\
"SYnA36O/Vi8h0APRePcb6PiGuFvlg7/wAgW/qy4pe+RB2MLIiX3mLkl2c6erQJmr\n"\
"wMQpzd6KVw8plF17fLfFfp/LDuyMuAPiJkiwWiYWQ9cOM0tqY73P6350X2qf0z88\n"\
"A+JnHK1AZCmrdSYYvUq/LQ8dBsKmsJuThBpx3tSsuBPzxPwPcA7vH1u4fUuXQiol\n"\
"2DRJPVkH73CZLNfkeJFJgfqKqUm2ntk3uBwjBnw+L0a/PTd5o+ZKzC52ofw6S0K3\n"\
"koHVbe3UMXeR44eZvXd0/mF/a+DMo+fK9CopPc+JwrOas/9Uls1LUBGwO2dpGVGv\n"\
"nukCAwEAAaNgMF4wHwYDVR0jBBgwFoAU2ma2TMW8a5MLtVmfpH8NosaL3OIwHQYD\n"\
"VR0OBBYEFMroz5blsOLy7q7Nue1bXPYJe3c5MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBHOdzSIZRLtMW2GSC0X5in9Jek\n"\
"skmhnXw2Gm/qxYCFixGWUetiDGh3XstVWYbnXWjbu9pisrM76DVad0lvWNb2Jd7x\n"\
"mpFC3ZXlK1kt+xZpofFm58IYHCO9weLFpx1JW/m6Bv+Ah6tycxnfadflgw/ITP0L\n"\
"Rx7iz+qwlXgy3uyJ2bd3uaC8uf4vny/fOQ8xJWsPTKNzEcMFP37DE0eOV6ct9ARQ\n"\
"r7gONE0up7pgelDDaZp8NbdPYMejxc+L8wkWmbU00qjeuaGB/djecrLjPdr10fTx\n"\
"7hgyrjolNg8zl0t5U2fylZRIhV8nCYrXqKYKCVDKtagXfuGL0TB5+cJmqaHd\n"\
"-----END CERTIFICATE-----\n"

/*
 * @brief PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 *
 * @todo If you are using AWS IoT Just in Time Registration (JITR), set this to
 * the issuer (Certificate Authority) certificate of the client certificate above.
 *
 * @note This setting is required by JITR because the issuer is used by the AWS
 * IoT gateway for routing the device's initial request. (The device client
 * certificate must always be sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html,
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set below to NULL.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM    ""

/*
 * @brief PEM-encoded client private key.
 *
 * @todo If you are running one of the Amazon FreeRTOS demo projects, set this
 * to the private key that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAy2qm48kRV4cH43phouJJicDfo79WLyHQA9F49xvo+Ia4W+WD\n"\
"v/ACBb+rLil75EHYwsiJfeYuSXZzp6tAmavAxCnN3opXDymUXXt8t8V+n8sO7Iy4\n"\
"A+ImSLBaJhZD1w4zS2pjvc/rfnRfap/TPzwD4mccrUBkKat1Jhi9Sr8tDx0Gwqaw\n"\
"m5OEGnHe1Ky4E/PE/A9wDu8fW7h9S5dCKiXYNEk9WQfvcJks1+R4kUmB+oqpSbae\n"\
"2Te4HCMGfD4vRr89N3mj5krMLnah/DpLQreSgdVt7dQxd5Hjh5m9d3T+YX9r4Myj\n"\
"58r0Kik9z4nCs5qz/1SWzUtQEbA7Z2kZUa+e6QIDAQABAoIBAGvKUDVUTsQ64YtB\n"\
"RouxrZY5hkiXE2xWVowvbBJFz+YiPX801+PVory5HFSR1vzVh69v5Ku6VbzH7/Fh\n"\
"ZhIIWVWZcWYDmK6IMf53eVdIaBj02SuwRbKJHBBYouybz8cd10CXV9M1YmzX7dfq\n"\
"aV6Qxt4qFaAZBvqtyJ4vbd81/wKKRTZdu6vi/cPDpxqY5Q9r7DKPO2fLBIBGmusn\n"\
"ANb/MSAvHs35sxIR754IMDHzZxHqJyh8Ki2S7feYv6c/BRTsLqq0Kxy1mKk6Iigt\n"\
"90tCuOBX/6dkJJv1ZYRV7Onbinamsi1MBINC2BUBxT2rw1d/Is/KkZlFPTBdktGB\n"\
"jajBtIECgYEA5o/HcnNmGbreuNvKCSi3qKbH7Yvmo22Mvp90ZBQDvzjaEi+KQnKB\n"\
"eyHxQb/wiuyq4e92ry1RVjxb6+8HqRS0i4Nrr7yYsYFB85pvO66nJx7kixIhXXb1\n"\
"8P/abXWONzhHyN5xScOE2+tiA3ovwCwKR1QWGMp35/zNkCdz3E28AwUCgYEA4dwp\n"\
"HtSfg+rqpHs3BD/4no5H0dZQZiUrXg/2boZqsSLTfQmVQc/DHXQyciDgATeeImMD\n"\
"HahD0+iS+jwTlJ1c0Qanebq0PyFcdnV9OI4m6qBxYy/iJ1R0Ei+O0+5CoBrg4ckb\n"\
"luOG0mw7gVcsHyqLzr+MzC4t4cJ6+L8hLdeq+ZUCgYBNLqaSzsTIKkFy2sjohgPt\n"\
"Zl+ZSvMblInAKnr5IaYbdnmyCs4a7mAgCz3LR+jLaOzPk61EeUQ4PR58fdkvpenG\n"\
"pmj/PIBVM7MTTIieDxqPwwy47YnT0wYD9nJ1oBvKqoGFYcnE6rnoMQTvqxCfIV8T\n"\
"f7eh9u1q8LsT3dEwV6lvzQKBgQC529wSvNFPUv8nvyU04BeNEnRTXtLmW2vmiag2\n"\
"p3IgthHm8P3cHEXpdPcWJ6tuFQCSN3wW6aJ3PQnF1a4fO6dnctxaNaOh16go6uHS\n"\
"qlen98OW5Y5q4vppBK26BHTbH4MzCbUkY7NMcTg+ZSevtek6hzIry0yCXPGoxE9f\n"\
"U/wkSQKBgDMGfFJUtPdPB3KYh+V6/DDnVG24ZVjhfCFhrLaNft4Nzh5SF4Eb6FuO\n"\
"oRwvy52HAefiQ8scxvkVN7C0R780AdtH0ImfONZispaBilWDrM76v2ALkUupOx53\n"\
"ZMyvMbhl/eEhCsCyYRDJjtFOdBjHRZ8mh7J/bdJIAy8A3SZ/8iYT\n"\
"-----END RSA PRIVATE KEY-----\n"



#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
