#include "Crypto.h"

bool md_sha256_t::operator==(const md_sha256_t &rhs) const
{
	return this->toString() == rhs.toString();
}


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


std::string Crypto::passwordSaltedHash(std::string salt, std::string password) const
{
	const int iterations = 100000;
	const int hash_length = 32;
	unsigned char hash[hash_length];
	if(1 != PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), (unsigned char*) salt.c_str(), salt.length(),
								iterations, EVP_sha256(), hash_length, hash))
	{
		fprintf(stderr, "Crypto::passwordSaltedHash failed.");
		handleError();
		return std::string();
	}
	
	return std::string((char*)hash, hash_length);
}

std::string Crypto::base64_encode(const ::std::string &bindata)
{
   using ::std::string;
   using ::std::numeric_limits;

   if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u) {
      throw ::std::length_error("Converting too large a string to base64.");
   }

   const ::std::size_t binlen = bindata.size();
   string retval((((binlen + 2) / 3) * 4), '=');
   ::std::size_t outpos = 0;
   int bits_collected = 0;
   unsigned int accumulator = 0;
   const string::const_iterator binend = bindata.end();

   for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
      accumulator = (accumulator << 8) | (*i & 0xffu);
      bits_collected += 8;
      while (bits_collected >= 6) {
         bits_collected -= 6;
         retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
      }
   }
   if (bits_collected > 0) {
      assert(bits_collected < 6);
      accumulator <<= 6 - bits_collected;
      retval[outpos++] = b64_table[accumulator & 0x3fu];
   }
   assert(outpos >= (retval.size() - 2));
   assert(outpos <= retval.size());
   return retval;
}

std::string Crypto::base64_decode(const ::std::string &ascdata)
{
   using ::std::string;
   string retval;
   const string::const_iterator last = ascdata.end();
   int bits_collected = 0;
   unsigned int accumulator = 0;

   for (string::const_iterator i = ascdata.begin(); i != last; ++i) {
      const int c = *i;
      if (::std::isspace(c) || c == '=') {
         continue;
      }
      if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
         throw ::std::invalid_argument("This contains characters not legal in a base64 encoded string.");
      }
      accumulator = (accumulator << 6) | reverse_table[c];
      bits_collected += 6;
      if (bits_collected >= 8) {
         bits_collected -= 8;
         retval += (char)((accumulator >> bits_collected) & 0xffu);
      }
   }
   return retval;
}
