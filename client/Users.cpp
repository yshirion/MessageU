#include "Users.h"

Users::Users()
{

}

std::shared_ptr<OtherUser> Users::GetUserByName(const std::string& nameUser)
{
	return users[nameUser];
}

std::shared_ptr<OtherUser> Users::GetUserById(const std::array<uint8_t, UidSize>& userId)
{
	for (auto u : users)
	{
		//if found this userId in one of the users its return this user
		if (u.second && std::equal(userId.cbegin(), userId.cend(), u.second->GetUid().cbegin()))
		{
			return u.second;
		}
	}
	//if no one found 
	return nullptr;
}

void Users::AddUser(std::shared_ptr<OtherUser> newUser)
{
	//add new user to users list by name user for key 
	users[newUser->GetName()] = std::move(newUser);
}

std::ostream& operator<<(std::ostream& stream, const Users& usersList)
{
	stream << "Names Of Users :" << std::endl;
	for (auto u : usersList.users)
	{
		auto user = u.second;
		if (!user)
		{
			continue;
		}
		stream << user->GetName() << std::endl;

	}
	return stream;
}
