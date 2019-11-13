#pragma once
#include <string>
#include <stdint.h>
#include "cipher_params.h"

namespace GL
{
// Encrypt the message contained in input file with iv and key using DES in CBC mode
void DES_cbc_encrypt(const std::string& inputFile, const std::string& outputFile, uint64_t(&subkeys)[ITERATIONS], uint64_t &iv);
// Decrypt the message contained in input file with iv and key using DES in CBC mode
void DES_cbc_decrypt(const std::string& inputFile, const std::string& outputFile, uint64_t(&subkeys)[ITERATIONS], uint64_t &iv);

// Encrypt the message contained in input file with iv and key using DES in CBC mode
void DES_cbc_encrypt(const char *in, int inLen, char* out, int& outLen, uint64_t(&subkeys)[ITERATIONS], uint64_t &iv);
// Decrypt the message contained in input file with iv and key using DES in CBC mode
void DES_cbc_decrypt(const char *in, int length, char* out, int& outLen, uint64_t(&subkeys)[ITERATIONS], uint64_t &iv);

// Encrypt the message contained in input file with iv and key using DES in CBC mode
//   keyLen ivLen <=8
void DES_cbc_encrypt(const char *in, int inLen, char* out, int& outLen, const char* keydata, int keyLen, const char* ivdata, int ivLen);
// Decrypt the message contained in input file with iv and key using DES in CBC mode
//   keyLen ivLen <=8
void DES_cbc_decrypt(const char *in, int length, char* out, int& outLen, const char* keydata, int keyLen, const char* ivdata, int ivLen);

std::string StrToHex(const unsigned char *md/*字符串*/, int nLen/*转义多少个字符*/);
void HexToStr(const char* strHex, char* out, int& outLen);
}

