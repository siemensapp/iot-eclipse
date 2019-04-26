/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_http_client_libcurl.c
* @date     Aug 19, 2016
* @brief    This file contains test case functions to test http_client_libcurl module.
*
************************************************************************/

#include "unity.h"
#include "http_client_libcurl.h"
#include "configuration.h"
#include "memory.h"
#include "string_array.h"
#include "string_type.h"
#include "string_util.h"
#include "list.h"
#include "http_definitions.h"
#include "data_types.h"
#include "definitions.h"
#include "mock_http_response.h"
#include "mock_http_response.h"

configuration_t *configuration = MCL_NULL;
http_client_t *http_client = MCL_NULL;

void setUp(void)
{	
	MCL_NEW(configuration);
	configuration->proxy_hostname = MCL_NULL;
	configuration->proxy_username = MCL_NULL;
	configuration->proxy_password = MCL_NULL;
	configuration->proxy_domain = MCL_NULL;
	string_initialize_static("https://www.example.com", 0, &(configuration->mindsphere_hostname));
	string_initialize_static(
		"-----BEGIN CERTIFICATE-----\n"\
		"MIIHPTCCBSWgAwIBAgIBADANBgkqhkiG9w0BAQQFADB5MRAwDgYDVQQKEwdSb290\n"\
		"IENBMR4wHAYDVQQLExVodHRwOi8vd3d3LmNhY2VydC5vcmcxIjAgBgNVBAMTGUNB\n"\
		"IENlcnQgU2lnbmluZyBBdXRob3JpdHkxITAfBgkqhkiG9w0BCQEWEnN1cHBvcnRA\n"\
		"Y2FjZXJ0Lm9yZzAeFw0wMzAzMzAxMjI5NDlaFw0zMzAzMjkxMjI5NDlaMHkxEDAO\n"\
		"BgNVBAoTB1Jvb3QgQ0ExHjAcBgNVBAsTFWh0dHA6Ly93d3cuY2FjZXJ0Lm9yZzEi\n"\
		"MCAGA1UEAxMZQ0EgQ2VydCBTaWduaW5nIEF1dGhvcml0eTEhMB8GCSqGSIb3DQEJ\n"\
		"ARYSc3VwcG9ydEBjYWNlcnQub3JnMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIIC\n"\
		"CgKCAgEAziLA4kZ97DYoB1CW8qAzQIxL8TtmPzHlawI229Z89vGIj053NgVBlfkJ\n"\
		"8BLPRoZzYLdufujAWGSuzbCtRRcMY/pnCujW0r8+55jE8Ez64AO7NV1sId6eINm6\n"\
		"zWYyN3L69wj1x81YyY7nDl7qPv4coRQKFWyGhFtkZip6qUtTefWIonvuLwphK42y\n"\
		"fk1WpRPs6tqSnqxEQR5YYGUFZvjARL3LlPdCfgv3ZWiYUQXw8wWRBB0bF4LsyFe7\n"\
		"w2t6iPGwcswlWyCR7BYCEo8y6RcYSNDHBS4CMEK4JZwFaz+qOqfrU0j36NK2B5jc\n"\
		"G8Y0f3/JHIJ6BVgrCFvzOKKrF11myZjXnhCLotLddJr3cQxyYN/Nb5gznZY0dj4k\n"\
		"epKwDpUeb+agRThHqtdB7Uq3EvbXG4OKDy7YCbZZ16oE/9KTfWgu3YtLq1i6L43q\n"\
		"laegw1SJpfvbi1EinbLDvhG+LJGGi5Z4rSDTii8aP8bQUWWHIbEZAWV/RRyH9XzQ\n"\
		"QUxPKZgh/TMfdQwEUfoZd9vUFBzugcMd9Zi3aQaRIt0AUMyBMawSB3s42mhb5ivU\n"\
		"fslfrejrckzzAeVLIL+aplfKkQABi6F1ITe1Yw1nPkZPcCBnzsXWWdsC4PDSy826\n"\
		"YreQQejdIOQpvGQpQsgi3Hia/0PsmBsJUUtaWsJx8cTLc6nloQsCAwEAAaOCAc4w\n"\
		"ggHKMB0GA1UdDgQWBBQWtTIb1Mfz4OaO873SsDrusjkY0TCBowYDVR0jBIGbMIGY\n"\
		"gBQWtTIb1Mfz4OaO873SsDrusjkY0aF9pHsweTEQMA4GA1UEChMHUm9vdCBDQTEe\n"\
		"MBwGA1UECxMVaHR0cDovL3d3dy5jYWNlcnQub3JnMSIwIAYDVQQDExlDQSBDZXJ0\n"\
		"IFNpZ25pbmcgQXV0aG9yaXR5MSEwHwYJKoZIhvcNAQkBFhJzdXBwb3J0QGNhY2Vy\n"\
		"dC5vcmeCAQAwDwYDVR0TAQH/BAUwAwEB/zAyBgNVHR8EKzApMCegJaAjhiFodHRw\n"\
		"czovL3d3dy5jYWNlcnQub3JnL3Jldm9rZS5jcmwwMAYJYIZIAYb4QgEEBCMWIWh0\n"\
		"dHBzOi8vd3d3LmNhY2VydC5vcmcvcmV2b2tlLmNybDA0BglghkgBhvhCAQgEJxYl\n"\
		"aHR0cDovL3d3dy5jYWNlcnQub3JnL2luZGV4LnBocD9pZD0xMDBWBglghkgBhvhC\n"\
		"AQ0ESRZHVG8gZ2V0IHlvdXIgb3duIGNlcnRpZmljYXRlIGZvciBGUkVFIGhlYWQg\n"\
		"b3ZlciB0byBodHRwOi8vd3d3LmNhY2VydC5vcmcwDQYJKoZIhvcNAQEEBQADggIB\n"\
		"ACjH7pyCArpcgBLKNQodgW+JapnM8mgPf6fhjViVPr3yBsOQWqy1YPaZQwGjiHCc\n"\
		"nWKdpIevZ1gNMDY75q1I08t0AoZxPuIrA2jxNGJARjtT6ij0rPtmlVOKTV39O9lg\n"\
		"18p5aTuxZZKmxoGCXJzN600BiqXfEVWqFcofN8CCmHBh22p8lqOOLlQ+TyGpkO/c\n"\
		"gr/c6EWtTZBzCDyUZbAEmXZ/4rzCahWqlwQ3JNgelE5tDlG+1sSPypZt90Pf6DBl\n"\
		"Jzt7u0NDY8RD97LsaMzhGY4i+5jhe1o+ATc7iwiwovOVThrLm82asduycPAtStvY\n"\
		"sONvRUgzEv/+PDIqVPfE94rwiCPCR/5kenHA0R6mY7AHfqQv0wGP3J8rtsYIqQ+T\n"\
		"SCX8Ev2fQtzzxD72V7DX3WnRBnc0CkvSyqD/HMaMyRa+xMwyN2hzXwj7UfdJUzYF\n"\
		"CpUCTPJ5GhD22Dp1nPMd8aINcGeGG7MW9S/lpOt5hvk9C8JzC6WZrG/8Z7jlLwum\n"\
		"GCSNe9FINSkYQKyTYOGWhlC0elnYjyELn8+CkcY7v2vcB5G5l1YjqrZslMZIBjzk\n"\
		"zk6q5PYvCdxTby78dOs6Y5nCpqyJvKeyRKANihDjbPIky/qbn3BHLt4Ui9SyIAmW\n"\
		"omTxJBzcoTWcFbLUvFUufQb1nA5V9FrWk9p2rSVzTMVD\n"\
		"-----END CERTIFICATE-----\n", 0, &(configuration->mindsphere_certificate));

    configuration->mindsphere_port = 443;
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;

    http_client = MCL_NULL;
}

void tearDown(void)
{
	string_destroy(&(configuration->mindsphere_hostname));
	string_destroy(&(configuration->mindsphere_certificate));
    MCL_FREE(configuration);
}

/**
 * GIVEN : User provides valid configuration parameters.
 * WHEN  : Http client is initialized.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_001(void)
{
    E_MCL_ERROR_CODE result = http_client_initialize(configuration, &http_client);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_client_initialize() does not return MCL_OK.");

    http_client_destroy(&http_client);
}


//// INFO The following function is used to test the functionality of the http_client although it is not considered as unit test.
///**
// * GIVEN : Http client is initialized and an HTTP GET request is created.
// * WHEN  : http_client_send is called.
// * THEN  : MCL_OK is returned.
// */
//void test_send_001(void)
//{
//    http_response_t *http_response = MCL_NULL;
//    http_request_t *http_request = MCL_NULL;
//
//    // Initialize an http client.
//    http_client_initialize(configuration, &http_client);
//
//    // Create an http get request.
//    MCL_NEW(http_request);
//    http_request->method = MCL_HTTP_GET;
//    string_array_initialize(1, &(http_request->header));
//    char *uri = "/";
//    string_initialize_new(MCL_NULL, configuration->mindsphere_hostname->length + strlen(uri), &(http_request->uri));
//    //http_request->uri->buffer = malloc(configuration->mindsphere_hostname->length + strlen(uri) + 1);
//    strcpy(http_request->uri->buffer, configuration->mindsphere_hostname->buffer);
//    strcat(http_request->uri->buffer, uri);
//
//    // Send the request.
//    E_MCL_ERROR_CODE result = http_client_send(http_client, http_request, &http_response);
//
//    // Print the response.
//    for(int i=0;i<http_response->header->index;i++)
//    {
//        string_t *tmp = string_array_get(http_response->header, i);
//        fwrite(tmp->buffer, tmp->length, 1, stdout);
//        fwrite("\n", 1, 1, stdout);
//    }
//    fwrite(http_response->payload, http_response->payload_size, 1, stdout);
//
//    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_client_initialize() does not return MCL_OK.");
//
//    http_client_destroy(&http_client);
//}
