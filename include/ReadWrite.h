
#include "Message.h"
#include <string>

						class ReadWrite : public Message
						{
						private:
							std::string filename;

						public:
							ReadWrite(short opCode, const std::string &filename);
							virtual std::string getFilename();
						};

