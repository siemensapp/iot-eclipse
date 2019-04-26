/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_exchange.c
* @date     Oct 27, 2016
* @brief    An integration test for testing exchange functionality of the MCL.
*
************************************************************************/

#include "unity.h"
#include "mcl/mcl.h"
#include "definitions.h"
#include <string.h>
#include <stdlib.h>

#define MCL_FILE_EXCHANGE_ENABLED 1

mcl_configuration_t *configuration = MCL_NULL;
mcl_communication_t *communication = MCL_NULL;

char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
char *routing = "vnd.kuka.FingerprintAnalizer";
const char *authentication_key = "bLVyMaYiLFy8tf7S669+7lix8JK/n2A70D8XgwoMGpM=";

const char *meta_type = "item";
const char *meta_version = "1.0";

const char *payload_version = "00.00.01";
const char *correlation_id = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";

const char *version = "1.0";
const char *timestamp = "2016-04-26T08:06:25.317Z";
const char *timestamp_2 = "2016-04-26T08:07:25.317Z";
E_MCL_EVENT_SEVERITY severity = MCL_EVENT_SEVERITY_ERROR;
const char *description = "nice event";
const char *event_payload_type = "thresholdViolation";
const char *event_payload_version = "1.0.1";

const char *details_name = "message";
const char *details_value = "Critical oscillation level";

// To use in data source part.
char *data_source_name = "Main motor";
char *data_source_description = "Motor performance";

// To use in data point part.
char *data_point_id = "e50ab7ca-fd5d-11e5-8000-001b1bc14a1d";
char *data_point_name = "Temperature";
char *data_point_description = "Climate change";
char *data_point_type = "uint";
char *data_point_unit = "°C";

//J-
char *fiddler_certificate =
"-----BEGIN CERTIFICATE-----\n"\
"MIIDsjCCApqgAwIBAgIQbvQfnldQrKRBqEGPNinT2zANBgkqhkiG9w0BAQsFADBn\n"\
"MSswKQYDVQQLDCJDcmVhdGVkIGJ5IGh0dHA6Ly93d3cuZmlkZGxlcjIuY29tMRUw\n"\
"EwYDVQQKDAxET19OT1RfVFJVU1QxITAfBgNVBAMMGERPX05PVF9UUlVTVF9GaWRk\n"\
"bGVyUm9vdDAeFw0xNzAxMzExNDQ0MDFaFw0yMzAxMzExNDQ0MDFaMGcxKzApBgNV\n"\
"BAsMIkNyZWF0ZWQgYnkgaHR0cDovL3d3dy5maWRkbGVyMi5jb20xFTATBgNVBAoM\n"\
"DERPX05PVF9UUlVTVDEhMB8GA1UEAwwYRE9fTk9UX1RSVVNUX0ZpZGRsZXJSb290\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvWZEiI0vap/0NDtb46bK\n"\
"PEfpXuOnbZ++KwVvm84GCfCflN4l8fZm5Mrd5rAf2M77Ql4N4y7S2TG7l2ZLjzT5\n"\
"KxPPqgdxnf2ZV2lzsMw36FG2aI+Ub8/n4L0qupSlerWBtQzn+NYF0O1JXgdq2jd9\n"\
"Dfebc3QjTrH2jJtnPF+gCfVi+gDSxUzE/8zEp9EgfqkkcDzeEWlSq2/nE8aZKx4W\n"\
"sfVxch1wFYY90vgxa38bygKAC/p0JG3xiiWBCTTx2TJfw2Gf+WIRhC9GQmIuaxu6\n"\
"YBNT4iw+GaH84KKAkPiQVb4e0lHfgBIMFZ9yfgVCwl6wlLQHZQDMRgS+4agyc9Ar\n"\
"owIDAQABo1owWDATBgNVHSUEDDAKBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/\n"\
"AgEAMB0GA1UdDgQWBBQQdVTbAUv/aM9L6neV01lcWSW88DAOBgNVHQ8BAf8EBAMC\n"\
"AQYwDQYJKoZIhvcNAQELBQADggEBAFK5NEDKLxYvm8Ngmhjd1wuC/kH/4ahqP1BF\n"\
"t9J3WMZqNwbukr5Oxkvk7DbUbkoj+HGqRX4pldClCPQ4qXdrWJm+KjGTwqYR0nMd\n"\
"IrAeeWwENcZQC2Sf+JBtLPKY2WBR1CYpfCAjLb1/cER9WUOagIdycCHdxqOv6OHy\n"\
"0iuaxkIULMrDwOxZIi5g0YHarXLXz70lKvJCq3CCMJ8hMNE/uoPnRzyCMg2lQXIc\n"\
"dNP50xslM1iXr5rZ4iBlqgRWzA6sFPfOpU3xGppHqO01FoRqrqmcJ+I+X70c6rPB\n"\
"t4Lsc4TJ0KsL9UboJP+Y4BX0oRDxcoP+ngxA8BhdFPqy0VLdHYw=\n"\
"-----END CERTIFICATE-----";

char *mock_server_certificate =
"-----BEGIN CERTIFICATE-----\n"\
"MIIDqDCCApCgAwIBAgIEo9uk7jANBgkqhkiG9w0BAQsFADBiMRswGQYDVQQDDBJ3\n"\
"d3cubW9ja3NlcnZlci5jb20xEzARBgNVBAoMCk1vY2tTZXJ2ZXIxDzANBgNVBAcM\n"\
"BkxvbmRvbjEQMA4GA1UECAwHRW5nbGFuZDELMAkGA1UEBhMCVUswIBcNMTUwMjEz\n"\
"MTYzMDA2WhgPMjExNjAxMjAxNjMwMDZaMGIxGzAZBgNVBAMMEnd3dy5tb2Nrc2Vy\n"\
"dmVyLmNvbTETMBEGA1UECgwKTW9ja1NlcnZlcjEPMA0GA1UEBwwGTG9uZG9uMRAw\n"\
"DgYDVQQIDAdFbmdsYW5kMQswCQYDVQQGEwJVSzCCASIwDQYJKoZIhvcNAQEBBQAD\n"\
"ggEPADCCAQoCggEBAIslQgdbmc1UDLjtCsFVeFEPHZWWQsIJkcLCT15QJbLnMPEm\n"\
"pyiSo07DQ1xQk7WCBOWWBuhJt+MwEAK5QFR/MLf11xcUF26PkO0GO01lEPtS6M8n\n"\
"pXpNCYKFF+gZ0q1hmxZ08ZQD+6LAPnLx5XUKk/XvjEpKRz01Sza4nUhgc4QDnN+B\n"\
"CiamQ4fzhbwR6eTPm9Z266RYhK+oPBBN1AxE/+CwO0nV85n9EYimn5kfZqyideCQ\n"\
"3ZbjPzXgfXctC9cWxKK4eEdfjMRnxxwAVVPCJU59sMv6JjhDV3YKWX4UMevsc6bf\n"\
"Q/KOgVAoFJuFXbAiS+oE5/JOW0yU9fHngD7QWUMCAwEAAaNkMGIwHQYDVR0OBBYE\n"\
"FKmWp1h2Zhsn4FP97H977H4izXGkMA8GA1UdEwEB/wQFMAMBAf8wCwYDVR0PBAQD\n"\
"AgG2MCMGA1UdJQQcMBoGCCsGAQUFBwMBBggrBgEFBQcDAgYEVR0lADANBgkqhkiG\n"\
"9w0BAQsFAAOCAQEAFW+ARa1RmSjbX82aj5+4hAVb3hiVcEVfDD/yfSwEYP93yNgV\n"\
"u74z/a3eqrzQKWveKH+CTGaRij1WNAknswrBijSEppNKEebg/J/vpXNwaDNK49oi\n"\
"8zbNAWPOB/mRK8ogY0qWXUHzt69r6Cm6Jid1TLKlpuJ60REZT8kfQlbXaNjy2g0N\n"\
"W254k/MBugmBQcKEJ/nfiiZe/1zLkn2JLQAgFf5LIvhGNBS/t7Fz0QiKwyfhyjP3\n"\
"t0YCdqheMQM8/rFjufCgJDu7IfieKguRoyRRbfSb6/gApANG3ARlLu6SoXK1TjfX\n"\
"gJvTmKj67HcgyjQgsSTx7r1O/HgxF4+rhpOpxg==\n"\
"-----END CERTIFICATE-----";
//J+

void setUp(void)
{
    mcl_configuration_initialize(&configuration);
    configuration->mindsphere_hostname = "https://127.0.0.1";
    configuration->mindsphere_port = 1080;
    configuration->mindsphere_certificate = mock_server_certificate;
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "http://127.0.0.1";
    configuration->proxy_port = 8888;
    configuration->proxy_type = MCL_PROXY_HTTP;
    configuration->proxy_username = MCL_NULL;
    configuration->proxy_password = MCL_NULL;
    configuration->proxy_domain = MCL_NULL;
    configuration->max_http_payload_size = 16384;
    configuration->user_agent = "custom agent v1.0";
    configuration->initial_access_token = "InitialAccessToken";
	configuration->tenant = "br-smk1";
	configuration->store_path = MCL_NULL;

}

void tearDown(void)
{
    mcl_configuration_destroy(&configuration);
    mcl_communication_destroy(&communication);
}

static void file_util_fopen(const char *file_name, const char *mode, FILE **file_descriptor)
{
#if defined(WIN32) || defined(WIN64)
    fopen_s(file_descriptor, file_name, mode);
#else
    *file_descriptor = fopen(file_name, mode);
#endif
}

/**
 * GIVEN : An agent already onboarded and a custom data to exchange.
 * WHEN  : Custom data is to be exchanged.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_01(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    const char *version = "1.0";
    const char *type = "customType";
    mcl_uint8_t payload[] =
    {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
    };
    mcl_size_t payload_size = 10;

    mcl_custom_data_t *custom_data = MCL_NULL;
    code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

    mcl_custom_data_set_payload(custom_data, payload, payload_size);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Custom data can not be exchanged.");
}

/**
 * GIVEN : An agent already onboarded and a custom data to exchange.
 * WHEN  : Custom data is to be exchanged.
 * THEN  : HTTP Response 400 (Bad Request) is returned from the server and MCL returns MCL_BAD_REQUEST.
 */
void test_exchange_02(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=400;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    const char *version = "1.0";
    const char *type = "customType";
    mcl_uint8_t payload[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };
    mcl_size_t payload_length = 10;

    mcl_custom_data_t *custom_data = MCL_NULL;
    code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

    mcl_custom_data_set_payload(custom_data, payload, payload_length);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_BAD_REQUEST, code, "MCL can not respond to HTTP Bad Request response correctly.");
}

/**
 * GIVEN : An agent already onboarded and a time series to exchange.
 * WHEN  : time series is to be exchanged without routing information.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_03(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create time_series
    const char *version = "1.0";
    mcl_time_series_t *time_series = MCL_NULL;
    mcl_store_new_time_series(store, version, configuration_id, MCL_NULL, &time_series);

    // Create value_set.
    char *timestamp = "2016-04-26T08:06:25.317Z";
    mcl_time_series_value_set_t *value_set;
    mcl_time_series_new_value_set(time_series, timestamp, &value_set);

    // Add value_set.
    char *data_point_id = (char *)__FUNCTION__;
    char *value = "65";
    char *quality_code = "00000000";
    mcl_time_series_add_value(value_set, data_point_id, value, quality_code);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "time_series can not be exchanged.");
}

/**
 * GIVEN : An agent already onboarded and a time series to exchange.
 * WHEN  : time series is to be exchanged.
 * THEN  : HTTP Response 400 (Bad Request) is returned from the server and MCL returns MCL_BAD_REQUEST.
 */
void test_exchange_04(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=400;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create time_series
    const char *version = "1.0";
    mcl_time_series_t *time_series = MCL_NULL;
    mcl_store_new_time_series(store, version, configuration_id, MCL_NULL, &time_series);

    // Create value_set.
    char *timestamp = "2016-04-26T08:06:25.317Z";
    mcl_time_series_value_set_t *value_set;
    mcl_time_series_new_value_set(time_series, timestamp, &value_set);

    // Add value_set.
    char *data_point_id = (char *)__FUNCTION__;
    char *value = "65";
    char *quality_code = "00000000";
    mcl_time_series_add_value(value_set, data_point_id, value, quality_code);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_BAD_REQUEST, code, "time_series can not be exchanged.");
}

/**
 * GIVEN : An agent already onboarded and a file to exchange.
 * WHEN  : File is to be exchanged.
 * THEN  : HTTP Response 200 is returned from the server and MCL returns MCL_OK.
 */
void test_exchange_05(void)
{
#if MCL_FILE_EXCHANGE_ENABLED
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    // Generate a file to upload to server.
	const char *file_path = "file_to_upload.txt";
    const char *file_name = "file_to_upload.txt";
    const char *file_type = "text";
    FILE *file_to_upload;
    file_util_fopen(file_path, "w", &file_to_upload);
    fputs("This file is for the test case test_exchange_09.", file_to_upload);
    fclose(file_to_upload);

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Add file to store.
    mcl_file_t *file = MCL_NULL;
    mcl_store_new_file(store, "1.0", file_path, file_name, file_type, routing, &file);

    // Exchange store.

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);

    remove(file_path);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL can not upload a file to MindSphere.");
#else
    tearDown();
    TEST_IGNORE_MESSAGE("Ignored since File Exchange is not enabled.");
#endif
}

/**
 * GIVEN : An agent already onboarded and a custom data to exchange.
 * WHEN  : Custom data is to be exchanged.
 * WHEN  : payload_size = max_http_payload_size - size_of_fixed_part, required_empty_size in http message is equal to max_http_payload_size.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_06(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    configuration->max_http_payload_size = 16384;

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    mcl_custom_data_t *custom_data = MCL_NULL;
    const char *version = "1.0";
    const char *type = "customType";
    const char *name = "testkey";
    const char *value = __FUNCTION__;

    // size_of_fixed_part = 453 for this case.
    mcl_size_t size_of_fixed_part =
        strlen("\r\n--gnPwBl0eaKx2oUv8Ew5eYX\r\nContent-Type: multipart/related;boundary=FtKgPZIElZKimSoNR04CWK\r\n\r\n--FtKgPZIElZKimSoNR04CWK\r\nContent-Type: application/vnd.siemens.mindsphere.meta+json\r\n\r\n{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\"},\"payload\":{\"type\":\"customType\",\"version\":\"1.0\",\"details\":{\"testkey\":\"test_exchange_11\"}}}\r\n--FtKgPZIElZKimSoNR04CWK\r\nContent-Type: application/octet-stream\r\n\r\n\r\n--FtKgPZIElZKimSoNR04CWK--\r\n--gnPwBl0eaKx2oUv8Ew5eYX--");

    // When payload_size is equal to (max_http_payload_size - size_of_fixed_part), then required_empty_size is equal to max_http_payload_size.
    // -1 is special to custom_data to make it be equal to max http size.
    mcl_size_t payload_size = configuration->max_http_payload_size - size_of_fixed_part - 1;

    mcl_uint8_t *payload = malloc(payload_size);

    mcl_store_new_custom_data(store, version, type, routing, &custom_data);

    // Create details json object.
    mcl_json_t *details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &details);

    // Add string to details json object.
    mcl_json_util_add_string(details, name, value);

    mcl_custom_data_set_meta_details(custom_data, details);
    mcl_custom_data_set_payload(custom_data, payload, payload_size);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);
    free(payload);
    mcl_json_util_destroy(&details);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Custom data can not be exchanged.");
}

/**
 * GIVEN : An agent already onboarded and a time series to exchange.
 * WHEN  : time series is to be exchanged.
 * WHEN  : required_empty_size in http message is equal to max_http_payload_size -1.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_07(void)
{
    // Warning: All the strings in this case are arranged to set the payload size = max_http_payload_size -1. Please don't change.

	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    configuration->max_http_payload_size = 16384;

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create time_series
    mcl_time_series_t *time_series = MCL_NULL;
    const char *version = "1.0";

    mcl_store_new_time_series(store, version, configuration_id, routing, &time_series);

    // size_of_fixed_part = 622 for this case.
    mcl_size_t size_of_fixed_part =
        strlen("\r\n--xinqujbjmUEMbVgLXuFbNO\r\nContent-Type: multipart/related;boundary=NHVwlhfOk5fJKxFZrpBZDZ\r\n\r\n--NHVwlhfOk5fJKxFZrpBZDZ\r\nContent-Type: application/vnd.siemens.mindsphere.meta+json\r\n\r\n{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\"},\"payload\":{\"type\":\"standardTimeSeries\",\"version\":\"1.0\",\"details\":{\"configurationId\":\"e3217e2b-7036-49f2-9814-4c38542cd781\"}}}\r\n--NHVwlhfOk5fJKxFZrpBZDZ\r\nContent-Type: application/json\r\n\r\n[{\"timeStamp\":\"\",\"values\":[{\"dataPointId\":\"e50ab7ca-fd5d-11etest_exchange_14\",\"value\":\"650\",\"qualityCode\":\"00000000\"}]}]\r\n--NHVwlhfOk5fJKxFZrpBZDZ--\r\n--xinqujbjmUEMbVgLXuFbNO--");

    // When payload_size is equal to (max_http_payload_size - size_of_fixed_part), then required_empty_size is equal to size_of_fixed_part.
    mcl_size_t payload_size = configuration->max_http_payload_size - size_of_fixed_part - 1;

    mcl_time_series_value_set_t *value_set;
    mcl_time_series_new_value_set(time_series, timestamp, &value_set);

    char *data_point_id = "e50ab7ca-fd5d-11etest_exchange_14";

    // To extend the payload. 22 is the calculated value so that required_empty_size in http message is equal to max_http_payload_size -1
    char *value = malloc(payload_size);
    value[payload_size - 22] = '\0';

    char *quality_code = "00000000";

    mcl_time_series_add_value(value_set, data_point_id, value, quality_code);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);
    free(value);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "time series can not be exchanged.");
}

/**
 * GIVEN : An agent already onboarded and a time series to exchange.
 * WHEN  : time series is to be exchanged.
 * WHEN  : required_empty_size in http message is equal to max_http_payload_size.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_08(void)
{
    // Warning: All the strings in this case were preciously arranged to set the payload size = max_http_payload_size. Please don't change.

	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    configuration->max_http_payload_size = 16384;

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create time_series
    mcl_time_series_t *time_series = MCL_NULL;
    const char *version = "1.0";

    mcl_store_new_time_series(store, version, configuration_id, routing, &time_series);

    // size_of_fixed_part =  for this case.
    mcl_size_t size_of_fixed_part =
        strlen("\r\n--WBUjkhXrtgrkqUDOeD5SnP\r\nContent-Type: multipart/related;boundary=pzRa8xz0ngvbluO08HA6Ga\r\n\r\n--pzRa8xz0ngvbluO08HA6Ga\r\nContent-Type: application/vnd.siemens.mindsphere.meta+json\r\n\r\n{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\"},\"payload\":{\"type\":\"standardTimeSeries\",\"version\":\"1.0\",\"details\":{\"configurationId\":\"e3217e2b-7036-49f2-9814-4c38542cd781\"}}}\r\n--pzRa8xz0ngvbluO08HA6Ga\r\nContent-Type: application/json\r\n\r\n[{\"timeStamp\":\"\",\"values\":[{\"dataPointId\":\"e50ab7ca-fd5d-11etest_exchange_15\",\"value\":\"650\",\"qualityCode\":\"00000000\"}]}]\r\n--pzRa8xz0ngvbluO08HA6Ga--\r\n--WBUjkhXrtgrkqUDOeD5SnP--");

    // When payload_size is equal to (max_http_payload_size - size_of_fixed_part), then required_empty_size is equal to size_of_fixed_part.
    mcl_size_t payload_size = configuration->max_http_payload_size - size_of_fixed_part;

    mcl_time_series_value_set_t *value_set;
    mcl_time_series_new_value_set(time_series, timestamp, &value_set);

    char *data_point_id = "e50ab7ca-fd5d-11etest_exchange_15";

    // To extend the payload. 22 is the calculated value so that required_empty_size in http message is equal to max_http_payload_size -1
    char *value = malloc(payload_size);
    value[payload_size - 22] = '\0';

    char *quality_code = "00000000";

    mcl_time_series_add_value(value_set, data_point_id, value, quality_code);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);
    free(value);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "time series can not be exchanged.");
}

/**
 * GIVEN : An agent already onboarded and a business event to exchange.
 * WHEN  : event is to be exchanged without routing information.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_09(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create event.
    mcl_event_t *event = MCL_NULL;
    mcl_store_new_event(store, version, event_payload_type, event_payload_version, severity, timestamp, &event);

    code = mcl_communication_exchange(communication, store, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "event can not be exchanged.");

    mcl_store_destroy(&store);
}

/**
 * GIVEN : An agent already onboarded with RSA security profile and a custom data to exchange.
 * WHEN  : Custom data is exchanged.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_11(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    configuration->security_profile = MCL_SECURITY_RSA_3072;

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    const char *version = "1.0";
    const char *type = "customType";
    mcl_uint8_t payload[] =
    {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
    };
    mcl_size_t payload_size = 10;

    mcl_custom_data_t *custom_data = MCL_NULL;
    code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

    mcl_custom_data_set_payload(custom_data, payload, payload_size);

    code = mcl_communication_exchange(communication, store, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Custom data can not be exchanged.");

    mcl_store_destroy(&store);
}

/**
 * GIVEN : A store which is not streamable and contains a custom data with payload greater than
 *         maximum http request payload size.
 * WHEN  : The store is exchanged.
 * THEN  : MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE is returned.
 */
void test_exchange_12(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%";
    configuration->max_http_payload_size = 16384;

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    mcl_custom_data_t *custom_data = MCL_NULL;
    const char *version = "1.0";
    const char *type = "customType";
    const char *name = "test name";
    const char *value = __FUNCTION__;

    // size_of_fixed_part = 417 for this case.
    mcl_size_t size_of_fixed_part =
        strlen("\r\n--kdpUqYdmKtj0ad9dOTdRrL\r\nContent-Type: multipart/related;boundary=2yKnJtU51Eg2OUD7PnPtix\r\n\r\n--2yKnJtU51Eg2OUD7PnPtix\r\nContent-Type: application/vnd.siemens.mindsphere.meta+json\r\n\r\n{\"type\":\"chunk\",\"version\":\"1.0\",\"details\":{\"chunkSetId\":\"47AA7FA0-4D6B-48B2-97D8-1AA567F16BA3\",\"chunkNo\":0}}\r\n--2yKnJtU51Eg2OUD7PnPtix\r\nContent-Type: application/octet-stream\r\n\r\n\r\n--2yKnJtU51Eg2OUD7PnPtix--\r\n--kdpUqYdmKtj0ad9dOTdRrL--");

    // When payload_size is equal to (max_http_payload_size - size_of_fixed_part), then required_empty_size is equal to size_of_fixed_part + 1.
    mcl_size_t payload_size = configuration->max_http_payload_size - size_of_fixed_part;

    mcl_uint8_t *payload = malloc(payload_size);

    mcl_store_new_custom_data(store, version, type, routing, &custom_data);

    // For mock server to differentiate the test case.
    memcpy(payload, value, 16);

    // Create details json object.
    mcl_json_t *details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &details);

    // Add string to details json object.
    mcl_json_util_add_string(details, name, value);

    mcl_custom_data_set_meta_details(custom_data, details);
    mcl_custom_data_set_payload(custom_data, payload, payload_size);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);
    free(payload);
    mcl_json_util_destroy(&details);

    TEST_ASSERT_EQUAL(MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE, code);
}

/**
 * GIVEN : An agent already onboarded and a custom data to exchange.
 * WHEN  : Custom data is to be exchanged.
 * THEN  : MCL_REQUEST_TIMEOUT is returned due to delay in the server.
 */
void test_exchange_13(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;TIMEOUT=2200%";
    configuration->http_request_timeout = 2;

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    const char *version = "1.0";
    const char *type = "customType";
    mcl_uint8_t payload[] =
    {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
    };
    mcl_size_t payload_size = 10;

    mcl_custom_data_t *custom_data = MCL_NULL;
    code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

    mcl_custom_data_set_payload(custom_data, payload, payload_size);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_REQUEST_TIMEOUT, code, "HTTP client did not respond correctly.");
}

/**
 * GIVEN : An agent already onboarded.
 * WHEN  : mcl_communication_exchange() is called and the server's response is 401 unauthorized.
 * THEN  : MCL_UNAUTHORIZED is returned from mcl_communication_exchange().
 */
void test_exchange_14(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=401;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    const char *version = "1.0";
    const char *type = "customType";
    mcl_uint8_t payload[] =
    {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
    };
    mcl_size_t payload_size = 10;

    mcl_custom_data_t *custom_data = MCL_NULL;
    code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

    mcl_custom_data_set_payload(custom_data, payload, payload_size);

    code = mcl_communication_exchange(communication, store, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_UNAUTHORIZED, code, "mcl_communication_exchange() didn't return MCL_UNAUTHORIZED for 401 response of server.");

    mcl_store_destroy(&store);
}

/**
 * GIVEN : An agent already onboarded.
 * WHEN  : mcl_communication_exchange() is called and the server's response is 403 forbidden.
 * THEN  : MCL_FORBIDDEN is returned from mcl_communication_exchange().
 */
void test_exchange_15(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=403;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    const char *version = "1.0";
    const char *type = "customType";
    mcl_uint8_t payload[] =
    {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
    };
    mcl_size_t payload_size = 10;

    mcl_custom_data_t *custom_data = MCL_NULL;
    code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

    mcl_custom_data_set_payload(custom_data, payload, payload_size);

    code = mcl_communication_exchange(communication, store, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_FORBIDDEN, code, "mcl_communication_exchange() didn't return MCL_FORBIDDEN for 403 response of server.");

    mcl_store_destroy(&store);
}

/**
 * GIVEN : An agent already onboarded.
 * WHEN  : mcl_communication_exchange() is called and the server's response is 500 internal server error.
 * THEN  : MCL_SERVER_FAIL is returned from mcl_communication_exchange().
 */
void test_exchange_16(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=500;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create custom data
    const char *version = "1.0";
    const char *type = "customType";
    mcl_uint8_t payload[] =
    {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
    };
    mcl_size_t payload_size = 10;

    mcl_custom_data_t *custom_data = MCL_NULL;
    code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

    mcl_custom_data_set_payload(custom_data, payload, payload_size);

    code = mcl_communication_exchange(communication, store, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_SERVER_FAIL, code, "mcl_communication_exchange() didn't return MCL_INTERNAL_SERVER_FAIL for 500 response of server.");

    mcl_store_destroy(&store);
}

/**
 * GIVEN : An agent already onboarded and a data source configuration to exchange without optional arguments.
 * WHEN  : data source configuration is to be exchanged.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_17(void)
{
    configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create data source configuration.
    const char *version = "1.0";
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;
    mcl_store_new_data_source_configuration(store, version, &data_source_configuration);

    // Add data source to data_source_configuration.
    mcl_data_source_t *data_source = MCL_NULL;
    mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, MCL_NULL, MCL_NULL, &data_source);

    // Add data point to data source.
    mcl_data_source_configuration_add_data_point(data_source, data_point_id, data_point_name, MCL_NULL, data_point_type, data_point_unit, MCL_NULL);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_store_destroy(&store);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Data source configuration can not be exchanged.");
}

/**
 * GIVEN : An agent already onboarded and a data source configuration to exchange with optional arguments.
 * WHEN  : data source configuration is to be exchanged.
 * THEN  : HTTP Response 200 is returned from the server.
 */
void test_exchange_18(void)
{
    configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    mcl_communication_initialize(configuration, &communication);

    // Onboard.
    E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create data source configuration.
    const char *version = "1.0";
    mcl_data_source_configuration_t *data_source_configuration = MCL_NULL;
    mcl_store_new_data_source_configuration(store, version, &data_source_configuration);

    // Create custom data of data source.
    mcl_json_t *custom_data_of_data_source = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data_of_data_source);
    mcl_json_util_add_string(custom_data_of_data_source, "motorHeat", "low");
    mcl_json_util_add_string(custom_data_of_data_source, "motorEfficiency", "high");

    // Add data source to data_source_configuration.
    mcl_data_source_t *data_source = MCL_NULL;
    mcl_data_source_configuration_add_data_source(data_source_configuration, data_source_name, data_source_description, custom_data_of_data_source, &data_source);

    // Create custom data of data point.
    mcl_json_t *custom_data_of_data_point = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &custom_data_of_data_point);
    mcl_json_util_add_string(custom_data_of_data_point, "temperatureAccuracy", "low");
    mcl_json_util_add_string(custom_data_of_data_point, "temperatureValue", "medium");

    // Add data point to data source.
    mcl_data_source_configuration_add_data_point(data_source, data_point_id, data_point_name, data_point_description, data_point_type, data_point_unit, custom_data_of_data_point);

    code = mcl_communication_exchange(communication, store, NULL);

    mcl_json_util_destroy(&custom_data_of_data_source);
    mcl_json_util_destroy(&custom_data_of_data_point);
    mcl_store_destroy(&store);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Data source configuration can not be exchanged.");
}

/**
* GIVEN : An agent already onboarded and a custom data to exchange. Event list argument is null.
* WHEN  : Custom data is to be exchanged.
* THEN  : HTTP Response 200 is returned from the server.
*/
void test_exchange_19(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
		"SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

	// Initialize an MCL Communication Handle with the given configuration.
	mcl_communication_initialize(configuration, &communication);

	// Onboard.
	E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

	// Initialize a store.
	mcl_store_t *store = MCL_NULL;
	code = mcl_store_initialize(MCL_FALSE, &store);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

	// Create custom data
	const char *version = "1.0";
	const char *type = "customType";
	mcl_uint8_t payload[] =
	{
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
	};
	mcl_size_t payload_size = 10;

	mcl_custom_data_t *custom_data = MCL_NULL;
	code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

	mcl_custom_data_set_payload(custom_data, payload, payload_size);

	code = mcl_communication_exchange(communication, store, MCL_NULL);

	mcl_store_destroy(&store);

	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Custom data can not be exchanged.");
}

/**
* GIVEN : An agent already onboarded and a business event to exchange. Server returns empty payload.
* WHEN  : Event is to be exchanged without routing information.
* THEN  : HTTP Response 200 is returned from the server.
*/
void test_exchange_20(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
		"SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;EVENT_NUMBER=0;%";

	// Initialize an MCL Communication Handle with the given configuration.
	mcl_communication_initialize(configuration, &communication);

	// Onboard.
	E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

	// Initialize a store.
	mcl_store_t *store = MCL_NULL;
	code = mcl_store_initialize(MCL_FALSE, &store);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

	// Create event.
	mcl_event_t *event = MCL_NULL;
    mcl_store_new_event(store, version, event_payload_type, event_payload_version, severity, timestamp, &event);

	code = mcl_communication_exchange(communication, store, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Event can not be exchanged.");

	mcl_store_destroy(&store);
}

/**
* GIVEN : An agent already onboarded and a file to exchange.
* WHEN  : File which is larger than max_http_payload_size is to be exchanged.
* THEN  : MCL returns MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE.
*/
void test_exchange_21(void)
{
#if MCL_FILE_EXCHANGE_ENABLED
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
		"SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

	char *too_large_buffer = calloc(MCL_MAXIMUM_HTTP_PAYLOAD_SIZE + 1, 1);
	memset(too_large_buffer, 'B', MCL_MAXIMUM_HTTP_PAYLOAD_SIZE);

	// Generate a file to upload to server.
	const char *file_path = "file_to_upload.txt";
	const char *file_name = "file_to_upload.txt";
	const char *file_type = "text";
	FILE *file_to_upload;
	file_util_fopen(file_path, "w", &file_to_upload);
	fputs(too_large_buffer, file_to_upload);
	fclose(file_to_upload);

	// Initialize an MCL Communication Handle with the given configuration.
	mcl_communication_initialize(configuration, &communication);

	// Onboard.
	E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

	// Initialize a store.
	mcl_store_t *store = MCL_NULL;
	code = mcl_store_initialize(MCL_FALSE, &store);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

	// Add file to store.
	mcl_file_t *file = MCL_NULL;
	mcl_store_new_file(store, "1.0", file_path, file_name, file_type, routing, &file);

	// Exchange store.
	code = mcl_communication_exchange(communication, store, NULL);
	TEST_ASSERT_MESSAGE(code == MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE, "MCL did not return MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE.");

	mcl_store_destroy(&store);
	remove(file_path);
	free(too_large_buffer);

#else
	tearDown();
	TEST_IGNORE_MESSAGE("Ignored since File Exchange is not enabled.");
#endif
}

/**
* GIVEN : An agent already onboarded and user wants to upload two files.
* WHEN  : Sum of file sizes exceeds max_http_payload_size although each size is smaller than max_http_payload_size.
* THEN  : MCL returns MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE.
*/
void test_exchange_22(void)
{
#if MCL_FILE_EXCHANGE_ENABLED
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
		"SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";

	char *too_large_buffer = calloc((MCL_MAXIMUM_HTTP_PAYLOAD_SIZE / 2) + 5, 1);
	memset(too_large_buffer, 'B', (MCL_MAXIMUM_HTTP_PAYLOAD_SIZE / 2) + 4);

	// Generate a file to upload to server.
	const char *file_path = "file_to_upload.txt";
	const char *file_name = "file_to_upload.txt";
	const char *file_type = "text";
	FILE *file_to_upload;
	file_util_fopen(file_path, "w", &file_to_upload);
	fputs(too_large_buffer, file_to_upload);
	fclose(file_to_upload);

	// Initialize an MCL Communication Handle with the given configuration.
	mcl_communication_initialize(configuration, &communication);

	// Onboard.
	E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

	// Initialize a store.
	mcl_store_t *store = MCL_NULL;
	code = mcl_store_initialize(MCL_FALSE, &store);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

	// Add file to store.
	mcl_file_t *file = MCL_NULL;
	mcl_store_new_file(store, "1.0", file_path, file_name, file_type, routing, &file);
	mcl_store_new_file(store, "1.0", file_path, file_name, file_type, routing, &file);

	// Exchange store.
	code = mcl_communication_exchange(communication, store, NULL);
	TEST_ASSERT_MESSAGE(code == MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE, "MCL did not return MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE.");

	mcl_store_destroy(&store);
	remove(file_path);
	free(too_large_buffer);

#else
	tearDown();
	TEST_IGNORE_MESSAGE("Ignored since File Exchange is not enabled.");
#endif
}

/**
* GIVEN : An agent already onboarded and two events to exchange.
* WHEN  : mcl_coomunication_exchange is called.
* THEN  : HTTP Response 200 is returned from the server.
*/
void test_exchange_23(void)
{
    configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization fail.");

    // Onboard.
    code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create event.
    mcl_event_t *event = MCL_NULL;
    code = mcl_store_new_event(store, "1.0", event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Can not add first event to store.");

    mcl_event_t *event2 = MCL_NULL;
    code = mcl_store_new_event(store, "1.0", event_payload_type, event_payload_version, severity, timestamp, &event2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Can not add second event to store.");

    code = mcl_communication_exchange(communication, store, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "event can not be exchanged.");

    mcl_store_destroy(&store);
}

/**
* GIVEN : An agent already onboarded and six events to exchange.
* WHEN  : mcl_coomunication_exchange is called.
* THEN  : HTTP Response 200 is returned from the server.
*/
void test_exchange_24(void)
{
    configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
        "SERVICE=EXCHANGE;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization fail.");

    // Onboard.
    code = mcl_communication_onboard(communication);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

    // Initialize a store.
    mcl_store_t *store = MCL_NULL;
    code = mcl_store_initialize(MCL_FALSE, &store);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

    // Create event.
    mcl_event_t *event = MCL_NULL;
    code = mcl_store_new_event(store, "1.0", "TestType1", "1.0.1", severity, timestamp, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Can not add first event to store.");

    mcl_event_t *event2 = MCL_NULL;
    code = mcl_store_new_event(store, "1.0", "TestType1", "1.0.1", severity, timestamp_2, &event2);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Can not add second event to store.");

    mcl_event_t *event3 = MCL_NULL;
    code = mcl_store_new_event(store, "1.0", "TestType1", "1.1.2", severity, timestamp, &event3);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Can not add third event to store.");

    mcl_event_t *event4 = MCL_NULL;
    code = mcl_store_new_event(store, "1.0", "TestType1", "1.1.2", severity, timestamp_2, &event4);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Can not add fourth event to store.");

    mcl_event_t *event5 = MCL_NULL;
    code = mcl_store_new_event(store, "2.0", "TestType1", "1.1.2", severity, timestamp, &event5);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Can not add fifth event to store.");

    mcl_event_t *event6 = MCL_NULL;
    code = mcl_store_new_event(store, "2.0", "TestType1", "1.1.2", severity, timestamp_2, &event6);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Can not add sixth event to store.");

    code = mcl_communication_exchange(communication, store, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "event can not be exchanged.");

    mcl_store_destroy(&store);
}

/**
* GIVEN : An agent already onboarded and access token is expired.
* WHEN  : Custom data is to be exchanged with #mcl_communication_process() function.
* THEN  : After the first fail, access token is renewed automatically.
*/
void test_process_01(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
		"SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=401;%";

	// Initialize an MCL Communication Handle with the given configuration.
	mcl_communication_initialize(configuration, &communication);

	// Onboard.
	E_MCL_ERROR_CODE code = mcl_communication_onboard(communication);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboarding fail.");

	// Get token.
	char *first_token;
	code = mcl_communication_get_last_access_token(communication, &first_token);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Getting access token fail.");

	// Modify the token.
	first_token[0] = '-';

	// Initialize a store.
	mcl_store_t *store = MCL_NULL;
	code = mcl_store_initialize(MCL_FALSE, &store);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

	// Create custom data
	const char *version = "1.0";
	const char *type = "customType";
	mcl_uint8_t payload[] =
	{
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
	};
	mcl_size_t payload_size = 10;

	mcl_custom_data_t *custom_data = MCL_NULL;
	code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

	mcl_custom_data_set_payload(custom_data, payload, payload_size);

	mcl_communication_process(communication, store, NULL);

	// Get token.
	char *last_token;
	code = mcl_communication_get_last_access_token(communication, &last_token);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Getting access token fail.");

	// Check that access token is changed.
	mcl_size_t length = strlen(first_token);
	int result = strncmp(first_token, last_token, length);

	TEST_ASSERT_NOT_EQUAL_MESSAGE(0, result, "New token is not acquired.");
	mcl_store_destroy(&store);

	free(first_token);
	free(last_token);
}

/**
* GIVEN : An agent already onboarded and a custom data to exchange with NULL token and token endpoint returns 400.
* WHEN  : Custom data is to be exchanged with #mcl_communication_process() function.
* THEN  : First exchange try fails. Key is rotated since token endpoint returns 400.
*/
void test_process_02(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
		"SERVICE=ACCESS_TOKEN;HTTP_STATUS=400;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=401;%";
	configuration->store_path = "registrationInformationFile.txt";
	E_MCL_ERROR_CODE code;

	// Create a file that includes security information.
	FILE *file; 
	file = fopen("registrationInformationFile.txt", "w");
	fprintf(file, "s6BhdRkqt3\n");
	fprintf(file, "yLoG05o+5pCvlaP1zlhdmWIkOX5+rb6F+9Tr7I0fl4U=\n");
	fprintf(file, "test_exchange_37_RAT_nR5cCI6IkpXVCJ9.eyJpc3MiOiJTQ0kiLCJzdWIiOiJCMUEzOUY0REFFQjU0QTVCQTlDQUYwQTc3MDgzNTQ2QSIsImF1ZCI6Ik1DQVMiLCJpYXQiOjE1MDk2OTU1NTgsIm5iZiI6MTUwOTY5NTU1OCwiZXhwIjoxNTEwMzAwMzU4LCJqdGkiOiJmYzVmZWY1Ny04ZWZmLTRkM2MtYmFjNi0zNzE5ZTA5ZTlmZTYiLCJzY29wZSI6IlJBVCIsInNjaGVtYXMiOlsidXJuOnNpZW1lbnM6bWluZHNwaGVyZTp2MSJdLCJ0ZW4iOiJ0MC1zaWVtZW5zIiwiY2xpZW50X2NyZWRlbnRpYWxfcHJvZmlsZSI6IlNIQVJFRF9TRUNSRVQifQ.2xkN0o4xkJ1pKeOEjL3HX6ySDyxHJ-S5j8qM4QpXK_lB44brn_Kq2Ji0RcXTo4gKOX3A3zroCPwFowwj_ua_gveCXn1bLBPH2NK7rhxipz8prBU5dgOprCNcN-g5qoRuirobsKMyI-Igu6fbNv7i4Fo5l-DDp3xsv0xVdEQ_g8lEQdk0F0gK_7tetMV4G6MNK5HTOnVBomm_2Ys494s0NxPYy4RCjNZWVDe-DSzJeF4MxTCGGDD1SCRhCoxt5ZJN5SjsIdXQ_t0zJcR3g7DnaIiTGBJfumPrSfW8zP7meiBR3SFd-Ue6yjtEUjIoxrhA0TsrbRnsWWmLOX11FchBEGvmBMsL2VkcZ50xyc4NNJB3k5bN9R4E0rL-QMWEKN5OmpttJg70gumYxRlkgUdDMsXJyGLVO81pc_gfU01MgE5-TJBIdC08IY1HyhNN7n5t0yecP6U2bQMhKSKTl4Yn2k3NGh2rPktU_8WvenCsY0GmDbhjfqMoz9ibDokUpkcT\n");
	fprintf(file, "https://localhost/api/agentmanagement/v3/register/s6BhdRkqt3\n");
	fclose(file);

	// Initialize an MCL Communication Handle with the given configuration.
	mcl_communication_initialize(configuration, &communication);

	// Initialize a store.
	mcl_store_t *store = MCL_NULL;
	code = mcl_store_initialize(MCL_FALSE, &store);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

	// Create custom data
	const char *version = "1.0";
	const char *type = "customType";
	mcl_uint8_t payload[] =
	{
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
	};
	mcl_size_t payload_size = 10;

	mcl_custom_data_t *custom_data = MCL_NULL;
	code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

	mcl_custom_data_set_payload(custom_data, payload, payload_size);

	// Reading RAT from third line of the file before calling mcl_communication_process.
	file = fopen("registrationInformationFile.txt", "r");
	char first_rat[1000];
	for (int i = 0; i < 3; i++)
	{
		fgets(first_rat, sizeof(first_rat), file);
	}
	fclose(file);

	mcl_communication_process(communication, store, NULL);

	// Reading RAT from third line of the file after calling mcl_communication_process.
	file = fopen("registrationInformationFile.txt", "r");
	char last_rat[1000];
	for (int i = 0; i < 3; i++)
	{
		fgets(last_rat, sizeof(last_rat), file);
	}
	fclose(file);

	// Compare that RAT is changed.
	mcl_size_t length = strlen(first_rat);
	int result = strncmp(first_rat, last_rat, length);

	TEST_ASSERT_NOT_EQUAL_MESSAGE(0, result, "Key rotation failed.");

	remove("registrationInformationFile.txt");
	mcl_store_destroy(&store);
}

/**
* GIVEN : An agent already onboarded and a custom data to exchange with NULL token. 
* WHEN  : Custom data is to be exchanged with #mcl_communication_process() function.
* THEN  : New access token is obtained, exchange is done successfully.
*/
void test_process_03(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=KEY_ROTATION;HTTP_STATUS=200;%"\
		"SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=EXCHANGE;HTTP_STATUS=200;%";
	configuration->store_path = "registrationInformationFile.txt";
	E_MCL_ERROR_CODE code;

	// Create a file that includes security information.
	FILE *file;
	file = fopen("registrationInformationFile.txt", "w");
	fprintf(file, "s6BhdRkqt3\n");
	fprintf(file, "yLoG05o+5pCvlaP1zlhdmWIkOX5+rb6F+9Tr7I0fl4U=\n");
	fprintf(file, "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJTQ0kiLCJzdWIiOiJCMUEzOUY0REFFQjU0QTVCQTlDQUYwQTc3MDgzNTQ2QSIsImF1ZCI6Ik1DQVMiLCJpYXQiOjE1MDk2OTU1NTgsIm5iZiI6MTUwOTY5NTU1OCwiZXhwIjoxNTEwMzAwMzU4LCJqdGkiOiJmYzVmZWY1Ny04ZWZmLTRkM2MtYmFjNi0zNzE5ZTA5ZTlmZTYiLCJzY29wZSI6IlJBVCIsInNjaGVtYXMiOlsidXJuOnNpZW1lbnM6bWluZHNwaGVyZTp2MSJdLCJ0ZW4iOiJ0MC1zaWVtZW5zIiwiY2xpZW50X2NyZWRlbnRpYWxfcHJvZmlsZSI6IlNIQVJFRF9TRUNSRVQifQ.2xkN0o4xkJ1pKeOEjL3HX6ySDyxHJ-S5j8qM4QpXK_lB44brn_Kq2Ji0RcXTo4gKOX3A3zroCPwFowwj_ua_gveCXn1bLBPH2NK7rhxipz8prBU5dgOprCNcN-g5qoRuirobsKMyI-Igu6fbNv7i4Fo5l-DDp3xsv0xVdEQ_g8lEQdk0F0gK_7tetMV4G6MNK5HTOnVBomm_2Ys494s0NxPYy4RCjNZWVDe-DSzJeF4MxTCGGDD1SCRhCoxt5ZJN5SjsIdXQ_t0zJcR3g7DnaIiTGBJfumPrSfW8zP7meiBR3SFd-Ue6yjtEUjIoxrhA0TsrbRnsWWmLOX11FchBEGvmBMsL2VkcZ50xyc4NNJB3k5bN9R4E0rL-QMWEKN5OmpttJg70gumYxRlkgUdDMsXJyGLVO81pc_gfU01MgE5-TJBIdC08IY1HyhNN7n5t0yecP6U2bQMhKSKTl4Yn2k3NGh2rPktU_8WvenCsY0GmDbhjfqMoz9ibDokUpkcT\n");
	fprintf(file, "https://localhost/api/agentmanagement/v3/register/s6BhdRkqt3\n");
	fclose(file);

	// Initialize an MCL Communication Handle with the given configuration.
	mcl_communication_initialize(configuration, &communication);

	// Initialize a store.
	mcl_store_t *store = MCL_NULL;
	code = mcl_store_initialize(MCL_FALSE, &store);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store initialization fail.");

	// Create custom data
	const char *version = "1.0";
	const char *type = "customType";
	mcl_uint8_t payload[] =
	{
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
	};
	mcl_size_t payload_size = 10;

	mcl_custom_data_t *custom_data = MCL_NULL;
	code = mcl_store_new_custom_data(store, version, type, routing, &custom_data);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Adding custom data to store fail.");

	mcl_custom_data_set_payload(custom_data, payload, payload_size);

	code = mcl_communication_process(communication, store, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Custom data can not be exchanged.");

	remove("registrationInformationFile.txt");
	mcl_store_destroy(&store);
}
