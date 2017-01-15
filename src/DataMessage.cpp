#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/DataMessage.h"

namespace bgu
{
	namespace spl171
	{
		namespace net
		{
			namespace impl
			{
				namespace TFTP
				{
					namespace msg
					{

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
					}
				}
			}
		}
	}
}
