#pragma once

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

						/// <summary>
						/// Created by himelbrand on 1/9/17.
						/// </summary>
						class Message
						{
						protected:
							short opCode = 0;
							int packetSize = 0;
						public:
							Message(short opCode);
							virtual short getOpCode();
							virtual int getPacketSize();

						};

					}
				}
			}
		}
	}
}
