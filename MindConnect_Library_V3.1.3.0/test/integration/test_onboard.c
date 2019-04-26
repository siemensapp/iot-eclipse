/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_onboard.c
 * @date     Sep 20, 2016
 * @brief    An integration test for testing onboard functionality of the MCL.
 *
 ************************************************************************/

#include "unity.h"
#include "mcl/mcl.h"
#include "curl/curl.h"
#include "definitions.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define SHARED_SECRET_PARAMETER_COUNT (4)

mcl_configuration_t *configuration;
mcl_communication_t *communication = MCL_NULL;
static char *file_path;
static char *test_onboard_11_reg_info[SHARED_SECRET_PARAMETER_COUNT] = {NULL};

const char *authentication_key = "bLVyMaYiLFy8tf7S669+7lix8JK/n2A70D8XgwoMGpM=";

//J-
char *improper_certificate =
    "-----BEGIN CERTIFICATE-----\n"\
    "InvalidCertificateIQcgl5JcHmAqZNIheZNjIxcDANBgkqhkiG9w0BAQsFADBn\n"\
    "MSswKQYDVQQLDCJDcmVhdGVkIGJ5IGh0dHA6Ly93d3cuZmlkZGxlcjIuY29tMRUw\n"\
    "EwYDVQQKDAxET19OT1RfVFJVU1QxITAfBgNVBAMMGERPX05PVF9UUlVTVF9GaWRk\n"\
    "bGVyUm9vdDAeFw0xNTEwMjQxNzExNDBaFw0yMTEwMjMxNzExNDBaMGcxKzApBgNV\n"\
    "BAsMIkNyZWF0ZWQgYnkgaHR0cDovL3d3dy5maWRkbGVyMi5jb20xFTATBgNVBAoM\n"\
    "DERPX05PVF9UUlVTVDEhMB8GA1UEAwwYRE9fTk9UX1RSVVNUX0ZpZGRsZXJSb290\n"\
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtFdQpTZJ5r4sNmijBvJ5\n"\
    "ej3U0FusUNYhoqJyoB1nBivuKhet5VWgbULY+NohVMJp+K22/FtatwUwvToR36vh\n"\
    "ajpThc4ebHbdxmjisNJLIhMAEygt6C35bXFGhVHsbV8i7PWm3aSVjHUHUJq4gYkt\n"\
    "5Q9yi2RTjN+tYlZ+hHT98rBncdSwsGxr2jblaYKwFP4BGPBN08Wsoc4EpCtwgqKx\n"\
    "X1PoGyQQi8lXE1nVp8TczGKPeJ/0OpwPqg0D13iLux5xtKbDevmhuH3IOArrRG1H\n"\
    "iXQv+njVZBtg4ygj09MBXC6CbwzHwsklJu8DGxv45YZXWat4VG4PLSFUIMaafPAu\n"\
    "0QIDAQABo1owWDATBgNVHSUEDDAKBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/\n"\
    "AgEAMB0GA1UdDgQWBBTVSkiB4PW0HvNgDGQVT1B2oAz3NDAOBgNVHQ8BAf8EBAMC\n"\
    "AQYwDQYJKoZIhvcNAQELBQADggEBAJISTlO5o16MPJ6QOahXxYHo+spu3YB4rpJA\n"\
    "HwTf0ys/XnkOqWIvEkEViTkphYFtZ2fNH5pT3czYWPMJuh2SB3sEF2N27h9i7YP4\n"\
    "Rb0adptc+DkPDXAwC+CA+6whVHSaAasOXeIWq4CF1zPmPF9yTrGQZNldnT4P4PGP\n"\
    "d+Wt67LJLWJfwAW5oVTZlwS/JKZTclKhDYJTUek4srk1J5f/yT681HsUvHp08AvJ\n"\
    "AfAjUPm2CxFAwGwuFENo7QxE1m3ymBYTZVkbaS3XuGxYSjExdVatGekivj9Rvu4I\n"\
    "aWv490DonBQ63t+Sa9VoG9VDV43XU1xuH3AGTts+GMD17Gf/gwg=\n"\
    "-----END CERTIFICATE-----";

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

static E_MCL_ERROR_CODE _load_test_onboard_10(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);
static E_MCL_ERROR_CODE _save_test_onboard_10(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);

static E_MCL_ERROR_CODE _load_test_onboard_11(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);
static E_MCL_ERROR_CODE _save_test_onboard_11(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);

void setUp(void)
{
    mcl_configuration_initialize(&configuration);
    configuration->mindsphere_hostname = "https://localhost";
    configuration->mindsphere_port = 1080;
    configuration->mindsphere_certificate = mock_server_certificate;
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "http://127.0.0.1";
    configuration->proxy_port = 8888;
    configuration->proxy_type = MCL_PROXY_HTTP;
    configuration->proxy_username = MCL_NULL;
    configuration->proxy_password = MCL_NULL;
    configuration->proxy_domain = MCL_NULL;
    configuration->user_agent = "custom agent v1.0";
    configuration->initial_access_token = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=";
	configuration->tenant = "br-smk1";
    configuration->store_path = MCL_NULL;
}

void tearDown(void)
{
    mcl_configuration_destroy(&configuration);
    mcl_communication_destroy(&communication);
}

/**
 * GIVEN : Server certificate is not issued by Certificate Authority.
 * WHEN  : Agent tries to onboard.
 * THEN  : MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL is returned by MCL.
 */
void test_onboard_01(void)
{
    // invalid certificate
    configuration->mindsphere_certificate = fiddler_certificate;

    // initialize an mcl communication handle with the given configuration
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_EQUAL_MESSAGE(MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL, code, "Server did not respond to invalid certificate correctly.");
}

/**
 * GIVEN : Agent is not onboarded.
 * WHEN  : Agent tries to onboard.
 * THEN  : MCL_OK is returned indicating a successful onboard.
 */
void test_onboard_02(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Onboard failed.");
}

/**
 * GIVEN : Agent is already onboarded.
 * WHEN  : Agent tries to onboard again.
 * THEN  : No request is sent to the server, the library returns MCL_ALREADY_ONBOARDED.
 */
void test_onboard_03(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    // Try to onboard again.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_MESSAGE(MCL_ALREADY_ONBOARDED == code, "The agent is already onboarded but this can not be detected.");
}

/**
 * GIVEN : Improper certificate is provided.
 * WHEN  : Agent tries to onboard.
 * THEN  : MCL does not send the onboarding request to the server and returns MCL_IMPROPER_CERTIFICATE error.
 */
void test_onboard_04(void)
{
    // invalid certificate
    configuration->mindsphere_certificate = improper_certificate;

    // initialize an mcl communication handle with the given configuration
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_EQUAL_MESSAGE(MCL_IMPROPER_CERTIFICATE, code, "Server did not respond to improper certificate correctly.");
}

/**
* GIVEN : Agent is not onboarded. Security profile is RSA.
* WHEN  : Agent tries to onboard.
* THEN  : MCL_OK is returned indicating a successful onboard.
*/
void test_onboard_05(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%";

	configuration->security_profile = MCL_SECURITY_RSA_3072;

	// Initialize an MCL Communication Handle with the given configuration.
	E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

	// Onboard only if initialization is successful.
	if (MCL_OK == code)
	{
		code = mcl_communication_onboard(communication);
	}

	TEST_ASSERT_MESSAGE(MCL_OK == code, "Onboard failed.");
}

/**
* GIVEN : NULL parameter provided.
* WHEN  : Agent tries to onboard.
* THEN  : MCL does not send the onboarding request to the server and returns MCL_TRIGGERED_WITH_NULL error.
*/
void test_onboard_06(void)
{
	// Try to Onboard
	E_MCL_ERROR_CODE code = mcl_communication_onboard(MCL_NULL);

	TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL Triggered with NULL parameter.");
}

/**
* GIVEN : Agent is not onboarded.
* WHEN  : Agent tries to onboard with improper proxy host name.
* THEN  : MCL_COULD_NOT_RESOLVE_PROXY is returned.
*/
void test_onboard_07(void)
{
	configuration->proxy_hostname = "bgdtgdtrgbdrtjgr.com";

	// initialize an mcl communication handle with the given configuration
	E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

	// Onboard only if initialization is successful.
	if (MCL_OK == code)
	{
		code = mcl_communication_onboard(communication);
	}

	TEST_ASSERT_EQUAL_MESSAGE(MCL_COULD_NOT_RESOLVE_PROXY, code, "MCL Onboard triggered with improper host name.");
}

/**
* GIVEN : Agent is not onboarded.
* WHEN  : Agent tries to onboard with improper proxy port number.
* THEN  : MCL_COULD_NOT_CONNECT is returned.
*/
void test_onboard_08(void)
{
	configuration->proxy_port = 5555;

	// initialize an mcl communication handle with the given configuration
	E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

	// Onboard only if initialization is successful.
	if (MCL_OK == code)
	{
		code = mcl_communication_onboard(communication);
	}

	TEST_ASSERT_EQUAL_MESSAGE(MCL_COULD_NOT_CONNECT, code, "MCL Onboard triggered with improper proxy port number.");
}

/**
* GIVEN : Agent is not onboarded.
* WHEN  : Agent tries to onboard with Improper initial_registration_uri
* THEN  : HTTP Response 400 (Bad Request) is returned from the server and MCL returns MCL_BAD_REQUEST.
*/
void test_onboard_09(void)
{
	// initialize an mcl communication handle with the given configuration
	E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

	// Onboard only if initialization is successful.
	if (MCL_OK == code)
	{
		code = mcl_communication_onboard(communication);
	}
	TEST_ASSERT_EQUAL_MESSAGE(MCL_BAD_REQUEST, code, "MCL Onboard triggered with improper initial_registration_uri.");
}

/**
 * GIVEN : Agent is already onboarded and registration information is saved to a file in a standard manner.
 * WHEN  : Agent tries to onboard again using callback function.
 * THEN  : MCL_ALREADY_ONBOARDED is returned by MCL.
 */
void test_onboard_10(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%";

    file_path = (char*) __FUNCTION__;
    FILE *file = fopen(file_path, "r");

    if(MCL_NULL != file)
    {
        fclose(file);
        remove(file_path);
    }

    configuration->store_path = file_path;

    // initialize an mcl communication handle with the given configuration.
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Onboard failed.");

    mcl_communication_destroy(&communication);

    // Let MCL use the callback function to load registration info.
    configuration->load_function.shared_secret = (mcl_load_shared_secret_callback_t)_load_test_onboard_10;
    configuration->save_function.shared_secret = (mcl_save_shared_secret_callback_t)_save_test_onboard_10;

    code = mcl_communication_initialize(configuration, &communication);

    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_MESSAGE(MCL_ALREADY_ONBOARDED == code, "Usage of callback failed.");
}

/**
 * GIVEN : Agent is already onboarded and registration information is copied to memory using callback function.
 * WHEN  : Agent tries to onboard again using callback function.
 * THEN  : MCL_ALREADY_ONBOARDED is returned by MCL.
 */
void test_onboard_11(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%";

    configuration->store_path = NULL;

    // Let MCL use the callback function to load registration info.
    configuration->load_function.shared_secret = (mcl_load_shared_secret_callback_t)_load_test_onboard_11;
    configuration->save_function.shared_secret = (mcl_save_shared_secret_callback_t)_save_test_onboard_11;

    // initialize an mcl communication handle with the given configuration.
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Onboard failed.");

    mcl_communication_destroy(&communication);
    code = mcl_communication_initialize(configuration, &communication);

    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_MESSAGE(MCL_ALREADY_ONBOARDED == code, "Usage of callback failed.");
}

/**
 * GIVEN : Agent is not onboarded.
 * WHEN  : Agent tries to onboard.
 * THEN  : MCL_COULD_NOT_CONNECT is returned due to invalid proxy_port.
 */
void test_onboard_12(void)
{
    configuration->proxy_port = (mcl_uint16_t) 0xDEAD;
    configuration->http_request_timeout = 2;

    // initialize an mcl communication handle with the given configuration
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_EQUAL_MESSAGE(MCL_COULD_NOT_CONNECT, code, "HTTP client did not respond correctly.");
}

/**
 * GIVEN : Agent is onboarded.
 * WHEN  : Agent tries to get last token time.
 * THEN  : MCL_OK is returned indicating that getting last token time is successful.
 */
void test_onboard_13(void)
{
    configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%";

    // Initialize an MCL Communication Handle with the given configuration.
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Onboard failed.");

    char *token_time = NULL;
    code = mcl_communication_get_last_token_time(communication, &token_time);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Getting last token time failed.");
    TEST_ASSERT_NOT_NULL(token_time);
    free(token_time);
}

/**
 * GIVEN : Agent is onboarded.
 * WHEN  : Agent tries to get last token time.
 * THEN  : Getting last token time is failed.
 */
void test_onboard_14(void)
{
    configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
        "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;WITH_SERVER_TIME=FALSE%";

    // Initialize an MCL Communication Handle with the given configuration.
    E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

    // Onboard only if initialization is successful.
    if (MCL_OK == code)
    {
        code = mcl_communication_onboard(communication);
    }

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Onboard failed.");

    char *token_time;
    code = mcl_communication_get_last_token_time(communication, &token_time);

    TEST_ASSERT_MESSAGE(MCL_NO_SERVER_TIME == code, "MCL_NO_SERVER_TIME was expected.");
    TEST_ASSERT_NULL(token_time);
}

static E_MCL_ERROR_CODE _load_test_onboard_10(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri)
{
    FILE *file;
    char *buffer;
    mcl_size_t buffer_size = 1024;
    mcl_bool_t is_error_occured = MCL_FALSE;

    *client_id = NULL;
    *client_secret = NULL;
    *registration_access_token = NULL;
    *registration_uri = NULL;

    file = fopen(file_path, "r");
    if(NULL == file)
    {
        return MCL_REGISTRATION_INFO_IS_NOT_LOADED;
    }

    buffer = malloc(buffer_size);

    if(MCL_NULL == buffer)
    {
        fclose(file);
        return MCL_REGISTRATION_INFO_IS_NOT_LOADED;
    }

    char **parameter_array[] = {client_id, client_secret, registration_access_token, registration_uri};

    for(mcl_size_t i = 0; i < sizeof(parameter_array) / sizeof(void*); i++)
    {
        if(MCL_NULL != fgets(buffer, buffer_size, file))
        {
            size_t string_length = strlen(buffer);
            *(parameter_array[i]) = calloc(string_length + 1, 1);
            if(NULL == *(parameter_array[i]))
            {
                is_error_occured = MCL_TRUE;
                break;
            }
            memcpy(*(parameter_array[i]), buffer, string_length);
        }
        else
        {
            is_error_occured = MCL_TRUE;
            break;
        }
    }

    free(buffer);
    fclose(file);

    if(is_error_occured)
    {
        for(mcl_size_t i = 0; i < sizeof(parameter_array); i++)
        {
            if(NULL != *(parameter_array[i]))
            {
                free(*(parameter_array[i]));
                *(parameter_array[i]) = NULL;
            }
            else
            {
                break;
            }
        }

        return MCL_REGISTRATION_INFO_IS_NOT_LOADED;
    }

    return MCL_OK;
}

static E_MCL_ERROR_CODE _save_test_onboard_10(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
    // dummy callback
    return MCL_OK;
}

static E_MCL_ERROR_CODE _load_test_onboard_11(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri)
{
    // Test_onboard_11_reg_info[] must be NULL or valid pointers all together, just check one.
    if(NULL == test_onboard_11_reg_info[0])
    {
        *client_id = NULL;
        *client_secret = NULL;
        *registration_access_token = NULL;
        *registration_uri = NULL;
        return MCL_REGISTRATION_INFO_IS_NOT_LOADED;
    }

    *client_id = test_onboard_11_reg_info[0];
    *client_secret = test_onboard_11_reg_info[1];
    *registration_access_token = test_onboard_11_reg_info[2];
    *registration_uri = test_onboard_11_reg_info[3];

    return MCL_OK;
}

static E_MCL_ERROR_CODE _save_test_onboard_11(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
    const char * parameter_array[] = {client_id, client_secret, registration_access_token, registration_uri};
    for(uint8_t i = 0; i < SHARED_SECRET_PARAMETER_COUNT; i++)
    {
        test_onboard_11_reg_info[i] = malloc(strlen(parameter_array[i]) + 1);
        if(NULL == test_onboard_11_reg_info[i])
        {
            // Error case, free the allocated buffers.
            for(uint8_t j = 0; j < i; j++)
            {
                free(test_onboard_11_reg_info[j]);
                test_onboard_11_reg_info[j] = NULL;
            }
            return MCL_REGISTRATION_INFO_IS_NOT_SAVED;
        }
        memcpy(test_onboard_11_reg_info[i], parameter_array[i], strlen(parameter_array[i]) + 1);
    }

    return MCL_OK;
}
