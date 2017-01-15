#include "Disconnect.h"

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

						Disconnect::Disconnect() : Message((short) 10)
						{
							this->packetSize = 2;
						}
					}
				}
			}
		}
	}
}
