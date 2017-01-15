#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/Acknowledge.h"

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

						Acknowledge::Acknowledge(short blockNum) : Message((short) 4)
						{
							this->blockNum = blockNum;
							this->packetSize = 4;
						}

						short Acknowledge::getBlockNum()
						{
							return blockNum;
						}
					}
				}
			}
		}
	}
}
