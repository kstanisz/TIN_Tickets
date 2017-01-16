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
	
	bool operator==(const md_sha256_t &rhs) const;
	std::string toString() const;
};


class Crypto
{
private:
	static Crypto *instance_;	
	RSA *rsa_public_key;
	RSA *rsa_private_key;
	
	Crypto();
	void handleError() const;

public:
	static Crypto *instance()
	{
		if(!instance_)
			instance_ = new Crypto;
		return instance_;
	}
	~Crypto();
	md_sha256_t ticketChecksum(const Ticket &t) const;
	int readPublicKey(std::string filename = "pubkey.pem");
	int readPrivateKey(std::string filename = "privkey.pem");
	std::string rsaPublicEncrypt(std::string data);
	std::string rsaPrivateDecrypt(std::string data);
	
};

#endif //CRYPTO_H_
