#include "ReadWrite.h"

						ReadWrite::ReadWrite(short opCode, const std::wstring &filename) : Message(opCode)
						{
							this->filename = filename;
							this->packetSize = 3 + filename.getBytes()->length;
						}

						std::wstring ReadWrite::getFilename()
						{
							return filename;
						}
