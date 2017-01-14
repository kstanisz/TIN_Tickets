#include "Crypto.h"

std::string md_sha256_t::toString()
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
	
	// Clean RSA and keys
	if(rsa)
		RSA_free(rsa);
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


void Crypto::generateRsaKeys()
{
	// Clear existing keys	
	if(rsa)
		RSA_free(rsa);
        
	rsa = RSA_generate_key(2048, RSA_3, NULL, NULL);
	rsa_private_key = generateRsaKey(rsa);
    rsa_public_key = generateRsaKey(rsa);    
}


EVP_PKEY* Crypto::generateRsaKey(RSA *rsa)
{
    EVP_PKEY *key;
    key = EVP_PKEY_new();
    
    if(rsa && key && EVP_PKEY_assign_RSA(key, rsa))
    {
        // key owns RSA from now
        if(RSA_check_key(rsa) <= 0)
        {
            fprintf(stderr,"RSA_check_key failed.\n");
            handleError();
            EVP_PKEY_free(key);
            key = NULL;
        }
    }
    else
    {
        handleError();
        if(key)
        {
            EVP_PKEY_free(key);
            key = NULL;
		}
    }
    
    if(key == NULL)
		fprintf(stderr,"Crypto::generateRsaKey failed.\n");
    return key;
}


void Crypto::writePublicKey(std::string filename) const
{
	// Save the public key to .pem file
	if(rsa_public_key)
    {
		FILE *file;
        if((file = fopen(filename.c_str(), "wt")) && PEM_write_PUBKEY(file, rsa_public_key))
			fprintf(stderr,"Public key saved.\n");
		else
			handleError();
		if(file)
			fclose(file);
	}
    else
		fprintf(stderr,"Crypto::writePublicKey failed - the key doesn't exist.\n");
}


void Crypto::writePrivateKey(std::string filename) const
{
	// Save the private key to .pem file
	
	if(rsa_private_key)
    {
		FILE *file;
        if(file = fopen(filename.c_str(),"wt"))
        {
            if(!PEM_write_PrivateKey(file, rsa_private_key, NULL, NULL, 0, NULL, NULL))
            {
                fprintf(stderr,"Crypto::writePrivateKey failed - PEM_write_PrivateKey failed.\n");
                handleError();
            }
        }
        else
        {
            fprintf(stderr,"Crypto::writePrivateKey failed - cannot create \"%s\".\n", filename.c_str());
            handleError();            
        }
        if(file)
			fclose(file);
	}
	else
		fprintf(stderr,"Crypto::writePrivateKey failed - the key doesn't exist.\n");
}


void Crypto::readPublicKey(std::string filename)
{
	// Read the public key from file
	if(rsa_public_key)
		EVP_PKEY_free(rsa_public_key);
	FILE *file;
	if((file = fopen(filename.c_str(),"rt")) && (rsa_public_key = PEM_read_PUBKEY(file, NULL, NULL, NULL)))
		fprintf(stderr,"Public key read.\n");
	else
	{
		fprintf(stderr,"Crypto::readPublicKey failed - cannot read \"%s\".\n", filename.c_str());
		handleError();
	}
	if(file)
		fclose(file);
}

void Crypto::readPrivateKey(std::string filename)
{
	if(rsa_private_key)
		EVP_PKEY_free(rsa_private_key);

	FILE *file;
	if((file = fopen(filename.c_str(),"rt")) && (rsa_private_key = PEM_read_PrivateKey(file, NULL, NULL, NULL)))
		fprintf(stderr,"Private key read.\n");
	else
	{
		fprintf(stderr,"Crypto::readPrivateKey failed - cannot read \"%s\".\n", filename.c_str());
		handleError();
	}
	if(file)
		fclose(file);
}

std::string Crypto::rsaEncrypt(std::string message) const
{
	return message;
}

std::string Crypto::rsaDecrypt(std::string cipher) const
{
	return cipher;
}
