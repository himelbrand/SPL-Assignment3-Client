#include "DeleteFile.h"

						DeleteFile::DeleteFile(const std::wstring &filename) : Message((short) 8)
						{
							this->filename = filename;
							this->packetSize = 3 + filename.getBytes()->length;
						}

						std::wstring DeleteFile::getFilename()
						{
							return filename;
						}
