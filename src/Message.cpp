#include "Message.h"

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

						Message::Message(short opCode)
						{
							this->opCode = opCode;
						}

						short Message::getOpCode()
						{
							return opCode;
						}

						int Message::getPacketSize()
						{
							return packetSize;
						}
					}
				}
			}
		}
	}
}
