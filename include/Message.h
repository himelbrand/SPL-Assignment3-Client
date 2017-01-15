
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

