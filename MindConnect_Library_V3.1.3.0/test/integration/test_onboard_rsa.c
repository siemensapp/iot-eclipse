#include "unity.h"
#include "mcl/mcl.h"
#include "definitions.h"

mcl_configuration_t *configuration;
mcl_communication_t *communication = MCL_NULL;

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
* GIVEN : Agent is not onboarded. Security profile is RSA.
* WHEN  : Agent tries to onboard.
* THEN  : MCL_OK is returned indicating a successful onboard.
*/
void test_onboard_rsa_01(void)
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
