#include "User.h"

#pragma once
class OtherUser : public User
{
public:
	OtherUser(const std::string& name, std::array<uint8_t, UidSize>& uid);

	OtherUser(const OtherUser& ) = delete;
	OtherUser& operator= (const OtherUser& ) = delete;

	std::shared_ptr<CryptoPP::RSA::PublicKey> GetPublicKey();
	CryptoPP::byte* GetSymmetricKey();
	void SetPublicKey(std::shared_ptr<CryptoPP::RSA::PublicKey>);
	void SetSymmetricKey(CryptoPP::byte*);


private:
	std::shared_ptr<CryptoPP::RSA::PublicKey> public_key;
	std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> symmetric_key;
};

