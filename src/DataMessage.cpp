#include "DataMessage.h"

						DataMessage::DataMessage(short dataSize, short blockNum, std::vector<char> &data) : Message((short) 3)
						{
							this->dataSize = dataSize;
							this->blockNum = blockNum;
							this->data = data.clone();
							this->packetSize = 6 + data.size();

						}

						short DataMessage::getDataSize()
						{
							return dataSize;
						}

						short DataMessage::getBlockNum()
						{
							return blockNum;
						}

						std::vector<char> DataMessage::getData()
						{
							return data;
						}
