#include "Login.h"

						Login::Login(const std::wstring &username) : Message((short) 7)
						{
							this->username = username;
							this->packetSize = 3 + username.getBytes()->length;
						}

						std::wstring Login::getUsername()
						{
							return username;
						}
