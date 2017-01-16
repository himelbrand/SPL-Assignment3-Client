#ifndef DISCONNECT__
#define DISCONNECT__
#include "Message.h"
#include <string>

						class DeleteFile : public Message
						{
						private:
							std::wstring filename;
						public:
							DeleteFile(const std::wstring &filename);

							virtual std::wstring getFilename();
						};
