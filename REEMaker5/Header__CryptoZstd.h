#pragma once
#pragma comment (lib, "crypt32")
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
void handleErrors(void)
{
	ERR_print_errors_fp(stderr);
	abort();
}
int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key,
	unsigned char* iv, unsigned char* ciphertext)
{
	EVP_CIPHER_CTX* ctx;

	int len;

	int ciphertext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		handleErrors();



	/*
	 * Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits
	 */
	if (1 != EVP_EncryptInit_ex(ctx, EVP_chacha20(), NULL, key, iv))
		handleErrors();

	/*
	 * Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		handleErrors();
	ciphertext_len = len;

	/*
	 * Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
		handleErrors();
	ciphertext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}


int decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key,
	unsigned char* iv, unsigned char* plaintext)
{
	EVP_CIPHER_CTX* ctx;

	int len;

	int plaintext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		handleErrors();

	/*
	 * Initialise the decryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits
	 */
	if (1 != EVP_DecryptInit_ex(ctx, EVP_chacha20(), NULL, key, iv))
		handleErrors();

	/*
	 * Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary.
	 */
	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
		handleErrors();
	plaintext_len = len;

	/*
	 * Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		handleErrors();
	plaintext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
}


#include <zstd.h>
#include "zstd_common.h"
#include <xxh3.h>

enum byteBlock : uint8_t { Fichier = 1, PaquetDebut = 2, PaquetFin = 4 };
enum mesErreur : uint8_t { Conforme = 0, ErreurLecture = 1, ErreurEcriture = 2, ErreurCompression = 3, ErreurDecompression = 4, ErreurChaCha = 5, ErreurXXH3 = 6, ErreurAutre = 99 };

constexpr auto HeaderSize = 16;//szComp*4 + szUncomp*4 + szSHA256*8
uint16_t MEM_read16(const void* memPtr)
{
	uint16_t val; memcpy_s(&val, sizeof(val), memPtr, sizeof(val)); return val;
}
uint32_t MEM_read32(const void* memPtr)
{
	uint32_t val; memcpy_s(&val, sizeof(val), memPtr, sizeof(val)); return val;
}
uint64_t MEM_read64(const void* memPtr)
{
	uint64_t val; memcpy_s(&val, sizeof(val), memPtr, sizeof(val)); return val;
}
void MEM_write16(void* memPtr, uint16_t value)
{
	memcpy_s(memPtr, sizeof(value), &value, sizeof(value));
}
void MEM_write32(void* memPtr, uint32_t value)
{
	memcpy_s(memPtr, sizeof(value), &value, sizeof(value));
}
void MEM_write64(void* memPtr, uint64_t value)
{
	memcpy_s(memPtr, sizeof(value), &value, sizeof(value));
}

/*
		uint_16t :: 65535                :: FFFF                sz 2
		uint_32t :: 4294967295           :: FFFFFFFF            sz 4
		uint_64t :: 18446744073709551615 :: FFFFFFFFFFFFFFFF    sz 8
 */
 //TODO FREAD >> CHAR ARRAY >> CONV 2 INT

uint16_t FILE_read16(FILE* Fichier)
{
	std::vector<uint8_t> vec_uint16;
	vec_uint16.resize(sizeof(uint16_t));
	fread_s(&vec_uint16[0], sizeof(uint16_t), sizeof(uint8_t), sizeof(uint16_t), Fichier);
	uint16_t val; memcpy_s(&val, sizeof(val), &vec_uint16[0], sizeof(val));
	vec_uint16.clear();
	return val;
}
void FILE_write16(FILE* Fichier, uint16_t val)
{
	std::vector<uint8_t> vec_uint16;
	vec_uint16.resize(sizeof(uint16_t));
	MEM_write16(&vec_uint16[0], val);
	fwrite(&vec_uint16[0], sizeof(uint8_t), sizeof(uint16_t), Fichier);
	vec_uint16.clear();
	printf("");
}
uint32_t FILE_read32(FILE* Fichier)
{
	std::vector<uint8_t> vec_uint32;
	vec_uint32.resize(sizeof(uint32_t));
	fread_s(&vec_uint32[0], sizeof(uint32_t), sizeof(uint8_t), sizeof(uint32_t), Fichier);
	uint32_t val; memcpy_s(&val, sizeof(val), &vec_uint32[0], sizeof(val));
	vec_uint32.clear();
	return val;
}
void FILE_write32(FILE* Fichier, uint32_t val)
{
	std::vector<uint8_t> vec_uint32;
	vec_uint32.resize(sizeof(uint32_t));
	MEM_write32(&vec_uint32[0], val);
	fwrite(&vec_uint32[0], sizeof(uint8_t), sizeof(uint32_t), Fichier);
	vec_uint32.clear();
	printf("");
}
uint64_t FILE_read64(FILE* Fichier)
{
	std::vector<uint8_t> vec_uint64;
	vec_uint64.resize(sizeof(uint64_t));
	fread_s(&vec_uint64[0], sizeof(uint64_t), sizeof(uint8_t), sizeof(uint64_t), Fichier);
	uint64_t val; memcpy_s(&val, sizeof(val), &vec_uint64[0], sizeof(val));
	vec_uint64.clear();
	return val;
}
void FILE_write64(FILE* Fichier, uint64_t val)
{
	std::vector<uint8_t> vec_uint64;
	vec_uint64.resize(sizeof(uint64_t));
	MEM_write64(&vec_uint64[0], val);
	fwrite(&vec_uint64[0], sizeof(uint8_t), sizeof(uint64_t), Fichier);
	vec_uint64.clear();
	printf("");
}
//Retourne un chunk crypté avec les 8 premiers byte = taille compressé en uint64_t
//les 8 byte suivants = taille décompressé en uint64_t
//le reste = le chunk compressé et crypté
mesErreur CompressAndCryptChunk(std::vector<uint8_t>& inBytes, std::vector<uint8_t>& outBytes, int lvlComp, std::string key, std::string iv)
{
	//Calcul XXH3_64bits de la source
	XXH64_hash_t hash = XXH3_64bits(&inBytes[0], inBytes.size());
	if (hash == 0)
		return mesErreur::ErreurXXH3;

	//Compression ZSTD
	std::vector<unsigned char> BufferCOMP;
	BufferCOMP.resize(ZSTD_compressBound(inBytes.size() + HeaderSize));
	size_t const cpSize = ZSTD_compress(&BufferCOMP[HeaderSize], BufferCOMP.size(), &inBytes[0], inBytes.size(), lvlComp);
	if (cpSize < 0) {
		printf("Erreur compression");
		return mesErreur::ErreurCompression;
	}
	else
	{
		BufferCOMP.resize(cpSize + HeaderSize);
	}

	//Chiffrage CHACHA20
	outBytes.clear();
	outBytes.resize(BufferCOMP.size());
	int crSize = encrypt(&BufferCOMP[HeaderSize], BufferCOMP.size() - HeaderSize, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str(), &outBytes[HeaderSize]);
	if (crSize != outBytes.size() - HeaderSize)
	{
		printf("Erreur cryptage");
		return mesErreur::ErreurChaCha;
	}
	BufferCOMP.clear();
	MEM_write32(&outBytes[0], (uint32_t)cpSize);
	MEM_write32(&outBytes[4], (uint32_t)inBytes.size());
	MEM_write64(&outBytes[8], (uint64_t)hash /*XXH3_64bits*/);
	return mesErreur::Conforme;
}
mesErreur DecompressCryptChunk(std::vector<uint8_t>& inBytes, std::vector<uint8_t>& outBytes, uint32_t sizeComp, uint32_t sizeDecomp, uint64_t sXXH3_64, std::string key, std::string iv)
{
	std::vector<unsigned char> BufferDECRYPT;
	BufferDECRYPT.resize(inBytes.size());
	int decrSize = decrypt(&inBytes[0], inBytes.size(), (unsigned char*)key.c_str(), (unsigned char*)iv.c_str(), &BufferDECRYPT[0]);
	if (decrSize != inBytes.size())
	{
		printf("Erreur decryptage");
		return mesErreur::ErreurChaCha;
	}
	unsigned long long const frameSize = ZSTD_getFrameContentSize(&BufferDECRYPT[0], BufferDECRYPT.size());
	outBytes.clear();
	outBytes.resize(frameSize);
	int decSize = ZSTD_decompress(&outBytes[0], outBytes.size(), &BufferDECRYPT[0], BufferDECRYPT.size());
	if (decSize < 0) {
		printf("Erreur decompression");
		return mesErreur::ErreurDecompression;
	}
	BufferDECRYPT.clear();
	//Calcul XXH3_64bits du final
	XXH64_hash_t hash = XXH3_64bits(&outBytes[0], outBytes.size());
	if (hash != sXXH3_64 || hash == 0)
		return mesErreur::ErreurXXH3;
	return mesErreur::Conforme;
}
