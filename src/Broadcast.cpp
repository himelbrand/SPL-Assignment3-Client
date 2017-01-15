#include "Broadcast.h"

						Broadcast::Broadcast(char isAdded, const std::wstring &filename) : Message((short)9)
						{
							this->isAdded = isAdded;
							this->filename = filename;
							this->packetSize = 4 + filename.getBytes()->length;
						}

						char Broadcast::getIsAdded()
						{
							return isAdded;
						}

						std::wstring Broadcast::getFilename()
						{
							return filename;
						}
