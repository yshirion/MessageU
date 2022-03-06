#include "CryptoHelper.h"

bool CryptoHelper::EncryptUnsymmetricKey(std::shared_ptr <CryptoPP::RSA::PublicKey> public_key, const std::vector<uint8_t>& in, std::vector<uint8_t>* out)
{
	try
	{
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(*public_key);
		CryptoPP::VectorSource vs(in, true, new CryptoPP::PK_EncryptorFilter(rng, encryptor, new CryptoPP::VectorSink(*out)));
	}

	catch (const std::exception)
	{
		return false;
	}
	return true;
}

bool CryptoHelper::DecryptUnsymmetricKey(std::shared_ptr <CryptoPP::RSA::PrivateKey> private_key, const std::vector<uint8_t>& in, std::vector<uint8_t>* out)
{
	try
	{
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(*private_key);
		CryptoPP::VectorSource vs(in, true, new CryptoPP::PK_DecryptorFilter(rng, decryptor, new CryptoPP::VectorSink(*out)));
	}
	catch (const std::exception)
	{
		return false;
	}

	return true;
}

std::shared_ptr<CryptoPP::RSA::PublicKey> CryptoHelper::PublicKeyCreation(CryptoPP::byte* key_value)
{
	std::shared_ptr<CryptoPP::RSA::PublicKey> pub_key(new CryptoPP::RSA::PublicKey());
	CryptoPP::ArraySource as(key_value, Unsymmetric_Key_Length, true);
	pub_key->Load(as);
	return std::move(pub_key);
}

std::shared_ptr<CryptoPP::RSA::PublicKey> CryptoHelper::PublicKeyCreation(std::shared_ptr<CryptoPP::RSA::PrivateKey> priv_key)
{
	std::shared_ptr<CryptoPP::RSA::PublicKey> pub_key(new CryptoPP::RSA::PublicKey(*priv_key));
	return std::move(pub_key);
}

void CryptoHelper::ConvertUnsymmetricKeyToBytes(std::shared_ptr<CryptoPP::RSAFunction> key, CryptoPP::byte* key_bytes)
{
	CryptoPP::ArraySink as(key_bytes, Unsymmetric_Key_Length);
	key->Save(as);
}

std::shared_ptr<CryptoPP::RSA::PrivateKey> CryptoHelper::PrivateKeyCreation()
{
	CryptoPP::AutoSeededRandomPool rng;
	std::shared_ptr <CryptoPP::RSA::PrivateKey> priv_key(new CryptoPP::RSA::PrivateKey());
	priv_key->Initialize(rng, 1024);
	return std::move(priv_key);
}

std::shared_ptr<CryptoPP::RSA::PrivateKey> CryptoHelper::PrivateKeyCreation(CryptoPP::byte* key_value)
{
	std::shared_ptr<CryptoPP::RSA::PrivateKey> priv_key(new CryptoPP::RSA::PrivateKey());
	CryptoPP::ArraySource as(key_value, Unsymmetric_Key_Length, true);
	priv_key->Load(as);
	return std::move(priv_key);
}

bool CryptoHelper::EncryptSymmetricKey(CryptoPP::byte* key, const std::string& in, std::string* out)
{

	try
	{
		CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };
		CryptoPP::AES::Encryption aes_encryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption, iv);

		CryptoPP::StreamTransformationFilter stf_encryptor(cbc_encryption, new CryptoPP::StringSink(*out));
		stf_encryptor.Put(reinterpret_cast<const unsigned char*>(in.c_str()), in.length());
		stf_encryptor.MessageEnd();
	}
	catch (const std::exception)
	{
		return false;
	}

	return true;
}

bool CryptoHelper::DecryptSymmetricKey(CryptoPP::byte* key, const std::string& in, std::string* out)
{
	try
	{
		CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };
		CryptoPP::AES::Decryption aes_decryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
		CryptoPP::CBC_Mode_ExternalCipher::Decryption cbc_decryption(aes_decryption, iv);

		CryptoPP::StreamTransformationFilter stf_decryptor(cbc_decryption, new CryptoPP::StringSink(*out));
		stf_decryptor.Put(reinterpret_cast<const unsigned char*>(in.c_str()), in.size());
		stf_decryptor.MessageEnd();
	}
	catch (const std::exception)
	{
		return false;
	}

	return true;
}

void CryptoHelper::SymmetricKeyCreation(CryptoPP::byte* key)
{
	CryptoPP::AutoSeededRandomPool rnd;
	rnd.GenerateBlock(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
}

void CryptoHelper::ConvertUnsymmetricKeyTo64BaseString(std::shared_ptr<CryptoPP::RSAFunction> key, std::string* key_str)
{
	CryptoPP::Base64Encoder key_sink(new CryptoPP::StringSink(*key_str));
	key->DEREncode(key_sink);
	key_sink.MessageEnd();
}

std::shared_ptr<CryptoPP::RSA::PrivateKey> CryptoHelper::LoadUnsymmetricKeyFrom64BaseString(const std::string& key_str)
{
	std::shared_ptr<CryptoPP::RSA::PrivateKey> priv_key(new CryptoPP::RSA::PrivateKey);
	CryptoPP::ByteQueue bytes;
	CryptoPP::StringSource source(key_str, true, new CryptoPP::Base64Decoder);
	source.TransferTo(bytes);
	bytes.MessageEnd();
	priv_key->Load(bytes);
	return std::move(priv_key);
}
