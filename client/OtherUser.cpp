#include "OtherUser.h"


OtherUser::OtherUser(const std::string& name, std::array<uint8_t, UidSize>& uid)
:User(name,uid), public_key(NULLPTR) , symmetric_key{0}
{

}

std::shared_ptr<CryptoPP::RSA::PublicKey> OtherUser::GetPublicKey()
{
	return public_key;
}

CryptoPP::byte* OtherUser::GetSymmetricKey()
{
	// make sure the key has been set
	for (size_t i = 0; i < symmetric_key.size(); i++)
	{
		if (symmetric_key[i])
		{
			return symmetric_key.data();
		}
	}
	return nullptr;
}

void OtherUser::SetPublicKey(std::shared_ptr<CryptoPP::RSA::PublicKey> p_key)
{
	public_key = p_key;
}

void OtherUser::SetSymmetricKey(CryptoPP::byte* sym_key)
{
	std::copy(sym_key, sym_key + CryptoPP::AES::DEFAULT_KEYLENGTH, symmetric_key.begin());
}
