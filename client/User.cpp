#include "User.h"

User::User(const std::string& n , std::array<uint8_t, UidSize>& u) : name(n), uid(u)
{ }


const std::array<uint8_t, UidSize>& User::GetUid()
{
	return std::move(uid); 


}
std::string User::GetName() 
{
	return name; 
}

std::ostream& operator<<(std::ostream& output, const User& user)
{
	output << user.name;
	return output;
}