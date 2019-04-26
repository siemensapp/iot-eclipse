#include "unity.h"
#include "mcl/mcl.h"
#include "definitions.h"
#include <string.h>
#include <stdlib.h>

#define MCL_FILE_EXCHANGE_ENABLED 1

mcl_configuration_t *configuration;
mcl_communication_t *communication = MCL_NULL;

char *configuration_id = "e3217e2b-7036-49f2-9814-4c38542cd781";
char *routing = "vnd.kuka.FingerprintAnalizer";

const char *meta_type = "item";
const char *meta_version = "1.0";

const char *payload_version = "00.00.01";
const char *correlation_id = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";

const char *version = "1.0";
const char *timestamp = "2016-04-26T08:06:25.317Z";
E_MCL_EVENT_SEVERITY severity = MCL_EVENT_SEVERITY_ERROR;
const char *description = "nice event";
const char *status = "good status";
const char *event_payload_type = "thresholdViolation";
const char *event_payload_version = "1.0.1";

const char *details_name = "message";
const char *details_value = "Critical oscillation level";

//J-
char *fiddler_certificate =
"-----BEGIN CERTIFICATE-----\n"\
"MIIDsjCCApqgAwIBAgIQDRX1jqSXJbNJr1QK1jRkEzANBgkqhkiG9w0BAQsFADBn\n"\
"MSswKQYDVQQLDCJDcmVhdGVkIGJ5IGh0dHA6Ly93d3cuZmlkZGxlcjIuY29tMRUw\n"\
"EwYDVQQKDAxET19OT1RfVFJVU1QxITAfBgNVBAMMGERPX05PVF9UUlVTVF9GaWRk\n"\
"bGVyUm9vdDAeFw0xNjAzMDkxNDIxNDBaFw0yMjAzMDkxNDIxNDBaMGcxKzApBgNV\n"\
"BAsMIkNyZWF0ZWQgYnkgaHR0cDovL3d3dy5maWRkbGVyMi5jb20xFTATBgNVBAoM\n"\
"DERPX05PVF9UUlVTVDEhMB8GA1UEAwwYRE9fTk9UX1RSVVNUX0ZpZGRsZXJSb290\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1WlWhgyfkXczrHXSdoww\n"\
"pVL5dQ0g1MIG/aV/zwQyf8s3URZ8NVHDvmjmLuLeIj3oR2q6c6fzoiqBaZU5AcS9\n"\
"z41z5KvmKoDMSUrDnN1mDFSfVfJHmr08CDcPHCo65WhMN96KmMArs9/UUcC6KubU\n"\
"xQofsvF2DdUQuLw7Aq3FCGhbhep1h1lLo7mDG2/eJ3m2VqiFbytGXaCYY6YwihuX\n"\
"TuBj7Glp2ciWj+c9vX0R+c3HK3e/YwWESCgspdl06rHP8/YclBXliA/M8XdTHboZ\n"\
"lGHDr2H/dUfCsp5a6oDqXzHTvO0kn0+dK0ElZlMwThPg7o8JVQWz+YI4dx+TtwIc\n"\
"NQIDAQABo1owWDATBgNVHSUEDDAKBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/\n"\
"AgEAMB0GA1UdDgQWBBTNXcev2mCGXB2ZOSsQmAKdExgZhTAOBgNVHQ8BAf8EBAMC\n"\
"AQYwDQYJKoZIhvcNAQELBQADggEBAHq7wOaCyJA8UBY0Qzdty4trY2DRw3EbmWe9\n"\
"1cAndRwSZ1jg7+vdn2QB3CjLwfqF7TINgZ9Y3B3K3+VWC5tlvohYq5z9S/A+SRk+\n"\
"uc6T4/VxSu3bLeWPkrYQW2EQ2UdVjfHJOa08cBDEYsbMAybPCuNHKUK5VO148gHu\n"\
"86Uj3ggBYKHqY9BwwuiDOzY72UHqjgId0JLKKG+/qvDmWVDDAjXhDOOs9ASQIjQx\n"\
"MrfpJ0A3MgmAm9mFukOvQyAziqiD2ONztMnk28M5v13NW1wcV+43Gz/EUQzohMLg\n"\
"1jLQllVpkgQEjBJOaGNjVeKIYFCfT9zALn1hOhp7a+pQUEaqe1A=\n"\
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
* GIVEN : An agent already onboarded and a business event and a time series to exchange.
* WHEN  : Event & time series are to be exchanged.
* THEN  : MCL_OK returned due to HTTP Response 200 from the server.
*/
void test_exchange_mixed_type_01(void)
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

	// Create time_series.
    const char *version = "1.0";
	mcl_time_series_t *time_series = MCL_NULL;
	mcl_store_new_time_series(store, version, configuration_id, MCL_NULL, &time_series);

	// Create value_set.
	char *timestamp = "2016-04-26T08:06:25.317Z";
	mcl_time_series_value_set_t *value_set;
	mcl_time_series_new_value_set(time_series, timestamp, &value_set);

	// Add value_set.
	char *time_series_id = (char *)__FUNCTION__;
	char *value = "65";
	char *quality_code = "00000000";

	mcl_time_series_add_value(value_set, time_series_id, value, quality_code);
	code = mcl_communication_exchange(communication, store, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Store which contains a business event and a time series can not be exchanged.");

	mcl_store_destroy(&store);
}

/**
* GIVEN : An agent is onboarded and a custom data and a file to exchange.
* WHEN  : Custom data and a file are to be exchanged.
* THEN  : MCL_OK returned due to HTTP Response 200 from the server.
*/
void test_exchange_mixed_type_02(void)
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

	// Create custom data.
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

	// Generate a file to upload to server.
	const char *file_path = "D:\\file_to_upload.txt";
    const char *file_name = "file_to_upload.txt";
    const char *file_type = "text";
	FILE *file_to_upload;
	file_util_fopen(file_name, "w", &file_to_upload);
	fputs("This file is for the test case test_exchange_mixed_type_03.", file_to_upload);
	fclose(file_to_upload);

	// Add file to store.
	mcl_file_t *file = MCL_NULL;
	mcl_store_new_file(store, "1.0", file_path, file_name, file_type, routing, &file);

	code = mcl_communication_exchange(communication, store, NULL);

	mcl_store_destroy(&store);

	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Custom data & file can not be exchanged.");
}
