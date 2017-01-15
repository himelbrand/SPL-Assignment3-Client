#include "Message.h"

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
