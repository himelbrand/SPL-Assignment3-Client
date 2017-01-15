#include "DirRequest.h"

						DirRequest::DirRequest() : Message((short) 6)
						{
							this->packetSize = 2;
						}
