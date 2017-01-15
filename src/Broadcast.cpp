#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/Broadcast.h"

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
					}
				}
			}
		}
	}
}
