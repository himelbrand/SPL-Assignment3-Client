#include "Acknowledge.h"

						Acknowledge::Acknowledge(short blockNum) : Message((short) 4)
						{
							this->blockNum = blockNum;
							this->packetSize = 4;
						}

						short Acknowledge::getBlockNum()
						{
							return blockNum;
						}
