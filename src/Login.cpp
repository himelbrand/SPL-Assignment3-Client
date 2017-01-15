#include "Login.h"


Login::Login(const std::string &username) : Message((short) 7) {
	this->username = username;
	this->packetSize = 3 + username.getBytes()->length;
}

std::string Login::getUsername() {
	return username;
}

