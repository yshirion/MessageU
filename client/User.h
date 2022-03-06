#pragma once
#include <string>
#include "Utilities.h"
class User
{
public:
	User(const std::string&, std::array<uint8_t, UidSize>&);

	User(const User&) = delete;
	User& operator= (const User&) = delete;

	const std::array<uint8_t, UidSize>& GetUid(); 
	std::string GetName();

	friend std::ostream& operator<<(std::ostream&, const User&);

private:
	std::array<uint8_t, UidSize> uid;
	std::string name;
};

