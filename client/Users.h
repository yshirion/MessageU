#include <iostream>
#include "OtherUser.h"

#pragma once
class Users
{
public:
	Users();

	Users(const Users&) = delete;
	Users& operator= (const Users&) = delete;

	std::shared_ptr<OtherUser> GetUserByName(const std::string&);

	std::shared_ptr<OtherUser> GetUserById(const std::array<uint8_t, UidSize>&);

	void AddUser(std::shared_ptr<OtherUser>);

	friend std::ostream& operator<<(std::ostream&, const Users&);

private:

	std::map<std::string, std::shared_ptr<OtherUser>> users;
};

