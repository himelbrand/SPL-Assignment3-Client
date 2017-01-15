
#include "Message.h"
#include <string>

						class Login : public Message
						{
						private:
							std::string username;
						public:
							Login(const std::string &username);
							virtual std::string getUsername();
						};
