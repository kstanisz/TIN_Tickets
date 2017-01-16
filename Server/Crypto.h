#include "Crypto.h"

std::string md_sha256_t::toString() const
{
	char buffer[65];
	for(int i = 0; i < 32; ++i)
		sprintf(buffer+i*2, "%02x", md[i]);
	return std::string(buffer, 64);
}

Crypto *Crypto::instance_ = 0;		// Singleton architecture

Crypto::Crypto()
{
	// Load the human readable error strings for libcrypto
	ERR_load_crypto_strings();
	// Load all digest and cipher algorithms
	OpenSSL_add_all_algorithms();
	// Load config file, and other important initialisation
	OPENSSL_config(NULL);
	// Set seed for random number generator
	RAND_load_file("/dev/urandom", 1024);
	
	rsa_public_key = NULL;
	rsa_private_key = NULL;
}


void Crypto::handleError() const
{
	ERR_print_errors_fp(stderr);
}


Crypto::~Crypto()
{
	// Removes all digests, ciphers, etc.
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();
	
	// Clean RSA keys
	if(rsa_public_key != NULL)
		RSA_free(rsa_public_key);
	if(rsa_private_key != NULL)
		RSA_free(rsa_private_key);
}


md_sha256_t Crypto::ticketChecksum(const Ticket &t) const
{
	md_sha256_t checksum;
	unsigned int digest_len;
	std::string message = t.serialize();
	size_t message_size = message.size();
	const char *buffer = message.c_str();
	
	// Count SHA-2 (SHA256) checksum
	EVP_MD_CTX *mdctx;

	if((mdctx = EVP_MD_CTX_create()) == NULL)
		printf("ERROR: Crypto::ticketChecksum(): EVP_MD_CTX_create()\n");

	if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
		printf("ERROR: Crypto::ticketChecksum(): EVP_DigestInit_ex()\n");

	if(1 != EVP_DigestUpdate(mdctx, buffer, message_size))
		printf("ERROR: Crypto::ticketChecksum(): EVP_DigestUpdate()\n");

	if(1 != EVP_DigestFinal_ex(mdctx, checksum.md, &digest_len))
		printf("ERROR: Crypto::ticketChecksum(): EVP_DigestFinal_ex()\n");
		
	EVP_MD_CTX_destroy(mdctx);
	
	return checksum;
}


int Crypto::readPublicKey(std::string filename)
{
	// Read the public key from file
	FILE * fp = fopen(filename.c_str(),"rb");
 
    if(fp == NULL)
    {
        fprintf(stderr,"Crypto::readPublicKey failed - cannot open \"%s\".\n", filename.c_str());
        return -1;    
    }
    
    if(rsa_public_key != NULL)
		RSA_free(rsa_public_key);
 

    rsa_public_key = PEM_read_RSA_PUBKEY(fp, &rsa_public_key, NULL, NULL);
    if(rsa_public_key == NULL)
    {
		fprintf(stderr,"Crypto::readPublicKey failed - cannot load \"%s\".\n", filename.c_str());
		return -1;
	}
 
    return 1;
}


int Crypto::readPrivateKey(std::string filename)
{
	// Read the private key from file
	FILE * fp = fopen(filename.c_str(),"rb");
 
    if(fp == NULL)
    {
        fprintf(stderr,"Crypto::readPrivateKey failed - cannot open \"%s\".\n", filename.c_str());
        return -1;    
    }
    
    if(rsa_private_key != NULL)
		RSA_free(rsa_private_key);
 
    rsa_private_key = PEM_read_RSAPrivateKey(fp, &rsa_private_key, NULL, NULL);
    if(rsa_private_key == NULL)
    {
		fprintf(stderr,"Crypto::readPrivateKey failed - cannot load \"%s\".\n", filename.c_str());
		return -1;
	}
 
    return 1;
}


std::string Crypto::rsaPublicEncrypt(std::string data)
{
	if(rsa_public_key == NULL && (1 != readPublicKey()))
	{
		fprintf(stderr,"Crypto::rsaPublicEncrypt failed - no key loaded.\n");
		return std::string();
	}
	
	const char *data_buffer = data.c_str();
	unsigned char encrypted[RSA_size(rsa_public_key)];
	
	int size = RSA_public_encrypt(data.length(), (unsigned char*)data_buffer, encrypted, rsa_public_key, RSA_PKCS1_PADDING);
	if(size == -1)
	{
		fprintf(stderr,"Crypto::rsaPublicEncrypt failed.\n");
		handleError();
		return std::string();
	}
	
	return std::string((char*)encrypted, size);
}


std::string Crypto::rsaPrivateDecrypt(std::string data)
{
	if(rsa_private_key == NULL && (1 != readPrivateKey()))
	{
		fprintf(stderr,"Crypto::rsaPrivateEncrypt failed - no key loaded.\n");
		return std::string();
	}
	
	const char *data_buffer = data.c_str();
	unsigned char decrypted[RSA_size(rsa_private_key)];
	
	int size = RSA_private_decrypt(data.length(), (unsigned char*)data_buffer, decrypted, rsa_private_key, RSA_PKCS1_PADDING);
	if(size == -1)
	{
		fprintf(stderr,"Crypto::rsaPrivateEncrypt failed.\n");
		handleError();
		return std::string();
	}
	
	return std::string((char*)decrypted, size);
}
