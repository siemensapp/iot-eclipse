#include "unity.h"
#include "mcl/mcl.h"
#include "curl/curl.h"
#include "definitions.h"

mcl_configuration_t *configuration;
mcl_communication_t *communication = MCL_NULL;

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
	configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiIxYTQwODg0NS0xNmM3LTQzOGMtYjAyYS0xNzY4NmE1M2U1NTAiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yOVQwNToxNjoyMC40NzFaIiwibmJmIjoiMjAxNy0wMy0yOVQwNToxNjoyMC40NzFaIiwiZXhwIjoiMjAxNy0wNC0yNlQwNToxNjoyMC40NzFaIiwianRpIjoiMWE0MDg4NDUtMTZjNy00MzhjLWIwMmEtMTc2ODZhNTNlNTUwIiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.ccRfGJYUaJzUAoU8KcVXRgb2uPjDzST4SwlTFk2SJDA=";
	configuration->tenant = "br-smk1";
	configuration->store_path = MCL_NULL;
}

void tearDown(void)
{
	mcl_configuration_destroy(&configuration);
	mcl_communication_destroy(&communication);
}

/**
* GIVEN : Agent is onboarded. Security profile is RSA.
* WHEN  : Agent tries to rotate key.
* THEN  : MCL_OK is returned indicating a key rotation.
*/
void test_rotate_key_rsa_01(void)
{
	configuration->user_agent = "CONFIGURATION:SERVICE=ONBOARD;HTTP_STATUS=201;%"\
	    "SERVICE=ACCESS_TOKEN;HTTP_STATUS=200;%"\
		"SERVICE=KEY_ROTATION;HTTP_STATUS=200;%";

	configuration->security_profile = MCL_SECURITY_RSA_3072;

	// Initialize an MCL Communication Handle with the given configuration.
	E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

	// Onboard only if initialization is successful.
	if (MCL_OK == code)
	{
		code = mcl_communication_onboard(communication);
		TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Onboard failed.");
	}

	// Rotate key.
	if (MCL_OK == code)
	{
		code = mcl_communication_rotate_key(communication);
	}

	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Key rotation failed.");
}

/**
* GIVEN : An agent not onboarded yet.
* WHEN  : Agent tries to use its Registeration Access Token.
* THEN  : MCL does not send the request to server and automatically returns MCL_NOT_ONBOARDED.
*/
void test_rotate_key_rsa_02(void)
{
	// Initialize an MCL Communication Handle with the given configuration.
	E_MCL_ERROR_CODE code = mcl_communication_initialize(configuration, &communication);

	// Rotate key.
	if (MCL_OK == code)
	{
		code = mcl_communication_rotate_key(communication);
	}

	TEST_ASSERT_EQUAL_MESSAGE(MCL_NOT_ONBOARDED, code, "MCL does respond correctly to key rotation request of an agent not onboarded yet.");
}
