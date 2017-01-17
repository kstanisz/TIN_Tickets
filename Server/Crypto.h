#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <cstdio>
#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>
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
	const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	const char reverse_table[128] = {
	   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
	};
	
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
	std::string passwordSaltedHash(std::string salt, std::string password) const;
	std::string	base64_decode(const ::std::string &ascdata);
	std::string base64_encode(const ::std::string &bindata);
	
};

#endif //CRYPTO_H_
