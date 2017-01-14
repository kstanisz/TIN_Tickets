#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <cstdio>
#include <string>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include "DataStructures.h"

struct md_sha256_t
{
	unsigned char md[32];
	
	std::string toString();
};


class Crypto
{
private:
	static Crypto *instance_;	
	RSA *rsa;
	EVP_PKEY *rsa_public_key;
    EVP_PKEY *rsa_private_key;
	
	Crypto();
	void handleError() const;
	EVP_PKEY* generateRsaKey(RSA *rsa);

public:
	static Crypto *instance()
	{
		if(!instance_)
			instance_ = new Crypto;
		return instance_;
	}
	~Crypto();
	md_sha256_t ticketChecksum(const Ticket &t) const;
	void generateRsaKeys();
	void writePublicKey(std::string filename = "pubkey.pem") const;
	void writePrivateKey(std::string filename = "privkey.pem") const;
	void readPublicKey(std::string filename = "pubkey.pem");
	void readPrivateKey(std::string filename = "privkey.pem");
	std::string rsaEncrypt(std::string message) const;
	std::string rsaDecrypt(std::string cipher) const;
	
};

#endif //CRYPTO_H_
