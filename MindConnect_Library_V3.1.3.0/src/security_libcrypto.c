/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     security_libcrypto.c
 * @date     Jun 27, 2016
 * @brief    Security libcrypto module implementation file.
 *
 ************************************************************************/

#include "security_libcrypto.h"
#include "memory.h"
#include "definitions.h"
#include "log_util.h"
#include "base64.h"
#include "string_util.h"

#if (1 == HAVE_OPENSSL_BN_H_)
#include <openssl/bn.h>
#endif

#if (1 == HAVE_OPENSSL_BIO_H_)
#include <openssl/bio.h>
#endif

#if (1 == HAVE_OPENSSL_PEM_H_)
#include <openssl/pem.h>
#endif

#if (1 == HAVE_OPENSSL_RSA_H_)
#include <openssl/rsa.h>
#endif

#if (1 == HAVE_OPENSSL_MD5_H_)
#include <openssl/md5.h>
#endif

#if (1 == HAVE_OPENSSL_SHA_H_)
#include <openssl/sha.h>
#endif

#if (1 == HAVE_OPENSSL_CRYPTO_H_)
#include <openssl/crypto.h>
#endif

#if (1 == HAVE_OPENSSL_RAND_H_)
#include <openssl/rand.h>
#endif

#define KEY_LENGTH_BITS 3072

// This function is used for base64 encoding a big number such as RSA modulus, public exponent and private exponent.
static E_MCL_ERROR_CODE _base64_encode_big_number(BIGNUM *big_number, char **encoded);

// This function is used to get the public key in PEM format from OpenSSL RSA structure.
static E_MCL_ERROR_CODE _get_rsa_public_key(RSA *rsa, char **public_key);

// This function is used to get the private key in PEM format from OpenSSL RSA structure.
static E_MCL_ERROR_CODE _get_rsa_private_key(RSA *rsa, char **private_key);

void security_initialize(void)
{
    DEBUG_ENTRY("void")

    CRYPTO_set_mem_functions(memory_malloc, memory_realloc, memory_free);

    DEBUG_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE security_hash_sha256(const mcl_uint8_t *data, mcl_size_t data_size, mcl_uint8_t **hash, mcl_size_t *hash_size)
{
    DEBUG_ENTRY("const mcl_uint8_t *data = <%p>, mcl_size_t data_size = <%u>, mcl_uint8_t **hash = <%p>, mcl_size_t *hash_size = <%p>", data, data_size, hash, hash_size)

    *hash_size = 0;

    // allocate memory to store SHA256 result
    *hash = MCL_CALLOC(1, SHA256_DIGEST_LENGTH);
    ASSERT_CODE_MESSAGE(MCL_NULL != *hash, MCL_OUT_OF_MEMORY, "Memory allocation for SHA256 failed!");

    *hash_size = SHA256_DIGEST_LENGTH;

    // perform SHA256 calculation
    MCL_DEBUG("Calculating SHA256 with OpenSSL.");
    SHA256(data, data_size, *hash);

    MCL_DEBUG("Calculated SHA256 *hash = <%p>, hash_size = <%u>", *hash, hash_size);
    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE security_rsa_sign(char *rsa_key, char *data, mcl_size_t data_size, mcl_uint8_t **signature, mcl_size_t *signature_size)
{
    DEBUG_ENTRY("char *rsa_key = <%s>, char *data = <%s>, mcl_size_t data_size = <%u>, mcl_uint8_t **signature = <%p>, mcl_size_t *signature_size = <%p>", rsa_key, data, data_size, signature, signature_size)

	E_MCL_ERROR_CODE code = MCL_OK;
    unsigned int sign_length = 0;
	BIO* bio;
	RSA *rsa = MCL_NULL;
	mcl_uint8_t *hash = MCL_NULL;
	mcl_size_t hash_size = 0;
	mcl_bool_t ok;

	mcl_size_t length = string_util_strlen(rsa_key);
    bio = BIO_new_mem_buf(rsa_key, length);
    ASSERT_CODE_MESSAGE(MCL_NULL != bio, MCL_OUT_OF_MEMORY, "Memory can not be allocated for BIO.");

    rsa = PEM_read_bio_RSAPrivateKey(bio, &rsa, 0, MCL_NULL);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != rsa, BIO_free(bio), MCL_FAIL, "RSA structure can not be generated.");

    ok = (MCL_NULL != (*signature = MCL_MALLOC(RSA_size(rsa))));
    ok = ok && (MCL_OK == security_hash_sha256(data, data_size, &hash, &hash_size));
	ok = ok &&  (1 == RSA_sign(NID_sha256, hash, hash_size, *signature, &sign_length, rsa));
	*signature_size = sign_length;

	if (MCL_FALSE == ok)
    {
	    MCL_FREE(*signature);
	    code = MCL_FAIL;
	}

	MCL_FREE(hash);
    RSA_free(rsa);
    BIO_free(bio);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE security_generate_rsa_key(char **public_key, char **private_key)
{
	DEBUG_ENTRY("char **public_key = <%p>, char **private_key = <%p>", public_key, private_key)

	E_MCL_ERROR_CODE code = MCL_FAIL;
    RSA *rsa;
    BIGNUM *exponent = MCL_NULL;
    int result = 0;
    
    // Allocate for RSA structure.
    rsa = RSA_new();
    ASSERT_CODE_MESSAGE(MCL_NULL != rsa, MCL_OUT_OF_MEMORY, "OpenSSL RSA structure can not be allocated.");

    // Allocate for the public exponent (e).
    exponent = BN_new();
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != exponent, RSA_free(rsa), MCL_OUT_OF_MEMORY, "OpenSSL big number for exponent can not be allocated.");

    // Set the value of public exponent.
    BN_set_word(exponent, RSA_F4);

    // Generate RSA structure containing public/private key pair.
    result = RSA_generate_key_ex(rsa, KEY_LENGTH_BITS, exponent, MCL_NULL);

    // Free public exponent.
    BN_free(exponent);

    // Get private key.
    (0 != result) && (code = _get_rsa_private_key(rsa, private_key));

    // Get public key.
    (MCL_OK == code) && (code = _get_rsa_public_key(rsa, public_key));

    if (MCL_OK != code)
    {
        MCL_FREE(private_key);
        MCL_FREE(public_key);
    }
    RSA_free(rsa);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE security_rsa_get_modulus_and_exponent(char *public_key, char **modulus, char **exponent)
{
	DEBUG_ENTRY("char *public_key = <%s>, char **modulus = <%p>, char **exponent = <%p>", public_key, modulus, exponent)

	E_MCL_ERROR_CODE code;
    RSA *rsa = MCL_NULL;
    mcl_size_t length = string_util_strlen(public_key);
    BIO *bio = BIO_new_mem_buf(public_key, length);
    ASSERT_CODE_MESSAGE(MCL_NULL != bio, MCL_OUT_OF_MEMORY, "Memory can not be allocated for BIO.");

    rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, 0, MCL_NULL);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != rsa, BIO_free(bio), MCL_FAIL, "RSA structure can not be generated.");

    code = _base64_encode_big_number(rsa->n, modulus);
    (MCL_OK == code) && (code = _base64_encode_big_number(rsa->e, exponent));

    if (MCL_OK != code)
    {
        MCL_FREE(*modulus);
        MCL_FREE(*exponent);
    }

    RSA_free(rsa);
    BIO_free(bio);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE security_generate_random_bytes(unsigned char *buffer, mcl_size_t size)
{
    DEBUG_ENTRY("unsigned char *buffer = <%p>, mcl_size_t size = <%u>", buffer, size)

    E_MCL_ERROR_CODE code = MCL_FAIL;

#if (1 == HAVE_OPENSSL_RAND_H_)
    if(1 == RAND_bytes(buffer, (int)size))
    {
        code = MCL_OK;
    }
#endif

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _base64_encode_big_number(BIGNUM *big_number, char **encoded)
{
	DEBUG_ENTRY("BIGNUM *big_number = <%p>, char **encoded = <%p>", big_number, encoded)

	E_MCL_ERROR_CODE code;
	int binary_length;
	char *binary;
	string_t *binary_encoded = MCL_NULL;

	binary = MCL_MALLOC(BN_num_bytes(big_number));
    ASSERT_CODE(MCL_NULL != binary, MCL_OUT_OF_MEMORY);

    binary_length = BN_bn2bin(big_number, binary);

    code = base64_url_encode(binary, binary_length, &binary_encoded);

    if (MCL_OK == code)
    {
        *encoded = binary_encoded->buffer;
    }

    MCL_FREE(binary_encoded);
    MCL_FREE(binary);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _get_rsa_private_key(RSA *rsa, char **private_key)
{
	DEBUG_ENTRY("RSA *rsa = <%p>, char **private_key = <%p>", rsa, private_key)

    E_MCL_ERROR_CODE code = MCL_OK;
    BIO *bio;
    int key_length = 0;

    bio = BIO_new(BIO_s_mem());
    ASSERT_CODE_MESSAGE(MCL_NULL != bio, MCL_OUT_OF_MEMORY, "Memory can not allocated for BIO.");

    if (!PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL))
    {
        BIO_free_all(bio);
        MCL_ERROR_RETURN(MCL_FAIL, "Private key can not be written from RSA struct to BIO.");
    }
    key_length = BIO_pending(bio);

    *private_key = MCL_MALLOC(key_length + 1);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != *private_key, BIO_free_all(bio), MCL_OUT_OF_MEMORY, "Memory can not be allocated for private key.");

    if (key_length != BIO_read(bio, *private_key, key_length))
    {
        MCL_DEBUG("Private key can not be read from BIO.");
        code = MCL_FAIL;
        MCL_FREE(*private_key);
    }
    (*private_key)[key_length] = MCL_NULL_CHAR;

    BIO_free_all(bio);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _get_rsa_public_key(RSA *rsa, char **public_key)
{
	DEBUG_ENTRY("RSA *rsa = <%p>, char **public_key = <%p>", rsa, public_key)

    E_MCL_ERROR_CODE code = MCL_OK;
    BIO *bio;
    int key_length = 0;

    bio = BIO_new(BIO_s_mem());
    ASSERT_CODE_MESSAGE(MCL_NULL != bio, MCL_OUT_OF_MEMORY, "Memory can not allocated for BIO.");

    if (!PEM_write_bio_RSA_PUBKEY(bio, rsa))
    {
        BIO_free_all(bio);
        MCL_ERROR_RETURN(MCL_FAIL, "Public key can not be written from RSA struct to BIO.");
    }
    key_length = BIO_pending(bio);

    *public_key = MCL_MALLOC(key_length + 1);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != *public_key, BIO_free_all(bio), MCL_OUT_OF_MEMORY, "Memory can not be allocated for public key.");

    if (key_length != BIO_read(bio, *public_key, key_length))
    {
        MCL_DEBUG("Public key can not be read from BIO.");
        code = MCL_FAIL;
        MCL_FREE(*public_key);
    }
    (*public_key)[key_length] = MCL_NULL_CHAR;

    BIO_free_all(bio);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
