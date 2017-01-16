#ifndef ERROR__
#define ERROR__
#include "Message.h"
#include <string>

						class Error : public Message
						{
						private:
							short errorCode = 0;
							std::string errorMsg;
						public:
							Error(short errorCode);

							virtual short getErrorCode();

							virtual std::string getErrorMsg();
						};

#endif