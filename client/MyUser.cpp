#include "MyUser.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "CryptoHelper.h"

MyUser::MyUser(const std::string& name, std::array<uint8_t, UidSize>& uid) : User(name, uid)
{
	CryptoPP::AutoSeededRandomPool rng;
	std::shared_ptr <CryptoPP::RSA::PrivateKey> priv_key(new CryptoPP::RSA::PrivateKey());
	priv_key->Initialize(rng, 1024);

	private_key = std::move(priv_key);
	public_key = std::make_shared<CryptoPP::RSA::PublicKey>(*priv_key);
}

MyUser::MyUser(const std::string& name, std::array<uint8_t, UidSize>& uid, std::shared_ptr <CryptoPP::RSA::PrivateKey> priv_key) :
	User(name, uid)
{
	private_key = std::move(priv_key);
	public_key = std::move(CryptoHelper::PublicKeyCreation(private_key));
}

std::shared_ptr<CryptoPP::RSA::PublicKey> MyUser::GetPublicKey() 
{
	return public_key; 
}

std::shared_ptr<CryptoPP::RSA::PrivateKey> MyUser::GetPrivateKey() 
{
	return private_key; 
}

void MyUser::SaveMyUserToFile(std::shared_ptr<MyUser> user, const std::string& path)
{
	try {

		std::ofstream outfile(path);

		if (outfile.is_open())
		{
			outfile << user->GetName() << std::endl;
			for (auto ch : user->GetUid())
			{
				outfile << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(ch);
			}
			outfile << std::endl;
			std::string key_str;
			CryptoHelper::ConvertUnsymmetricKeyTo64BaseString(user->GetPrivateKey(), &key_str);
			outfile << key_str;
		}

		outfile.close();
	}
	catch (std::exception ex)
	{
		throw std::exception("error open file to save");
	}
}

std::shared_ptr<MyUser> MyUser::ReadMyUserFromFile(const std::string& path)
{
	std::string name, uid_string;
	std::ostringstream key_string;

	try {

		std::ifstream infile(path);
		if (!infile)
		{
			return nullptr;
		}

		if (infile.is_open())
		{
			std::getline(infile, name);
			std::getline(infile, uid_string);
			while (key_string << infile.rdbuf());
		}

		infile.close();
	}
	catch (std::exception ex)
	{
		std::cout << "error reading file " << path;
		return nullptr;
	}

	
	// parse user id from the file
	std::array<uint8_t, UidSize> uid;
	std::string next_pair(2, 0);
	for (size_t i = 0; i < uid.size(); i++)
	{
		std::copy(uid_string.cbegin() + i * 2, uid_string.cbegin() + (i * 2 + 2), next_pair.begin());
		uid[i] = std::stoul(next_pair, nullptr, 16);
	}

	auto private_key = CryptoHelper::LoadUnsymmetricKeyFrom64BaseString(key_string.str());

	return std::make_shared<MyUser>(name, uid, std::move(private_key));

}

