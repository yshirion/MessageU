#include "User.h"

#pragma once
class MyUser : public User
{
public:

	MyUser(const std::string&, std::array<uint8_t, UidSize>&);
	MyUser(const std::string&, std::array<uint8_t, UidSize>&, std::shared_ptr <CryptoPP::RSA::PrivateKey>);

	MyUser(const MyUser&) = delete;
	MyUser& operator= (const MyUser&) = delete;

	static std::shared_ptr<MyUser> ReadMyUserFromFile(const std::string&);
	static void SaveMyUserToFile(std::shared_ptr<MyUser>, const std::string&);
	std::shared_ptr<CryptoPP::RSA::PublicKey> GetPublicKey();
	std::shared_ptr<CryptoPP::RSA::PrivateKey> GetPrivateKey();



private:
	std::shared_ptr<CryptoPP::RSA::PublicKey> public_key;
	std::shared_ptr < CryptoPP::RSA::PrivateKey> private_key;

};

