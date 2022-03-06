#pragma once
#include <vector>
#include <memory>
#include <rsa.h>
#include <aes.h>
#include <filters.h>
#include <modes.h>
#include <osrng.h>
#include <base64.h>

/// <summary>
/// utility static calss includes functions to manage all encryption stuff
/// </summary>
static class CryptoHelper
{

public:
	//unsymmetric key
	static bool EncryptUnsymmetricKey(std::shared_ptr<CryptoPP::RSA::PublicKey>, const std::vector<uint8_t>&, std::vector<uint8_t>*);
	static bool DecryptUnsymmetricKey(std::shared_ptr<CryptoPP::RSA::PrivateKey>, const std::vector<uint8_t>&, std::vector<uint8_t>*);
	//symmetric key
	static bool EncryptSymmetricKey(CryptoPP::byte*, const std::string&, std::string*);
	static bool DecryptSymmetricKey(CryptoPP::byte*, const std::string&, std::string*);
	static void SymmetricKeyCreation(CryptoPP::byte*);
	//public key
	static std::shared_ptr<CryptoPP::RSA::PublicKey> PublicKeyCreation(CryptoPP::byte*);
	static std::shared_ptr<CryptoPP::RSA::PublicKey> PublicKeyCreation(std::shared_ptr<CryptoPP::RSA::PrivateKey>);
	//connvert
	static void ConvertUnsymmetricKeyToBytes(std::shared_ptr<CryptoPP::RSAFunction>, CryptoPP::byte*);
	static void ConvertUnsymmetricKeyTo64BaseString(std::shared_ptr<CryptoPP::RSAFunction>, std::string*);
	//private key
	static std::shared_ptr<CryptoPP::RSA::PrivateKey> PrivateKeyCreation();
	static std::shared_ptr<CryptoPP::RSA::PrivateKey> PrivateKeyCreation(CryptoPP::byte*);
	//load
	static std::shared_ptr<CryptoPP::RSA::PrivateKey> LoadUnsymmetricKeyFrom64BaseString(const std::string&);

	static const size_t Unsymmetric_Key_Length = 160;
	static const size_t Symmetric_Key_Length = CryptoPP::AES::DEFAULT_KEYLENGTH;
private:

};

