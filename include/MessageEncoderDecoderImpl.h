#pragma once

#include <string>
#include <vector>

//JAVA TO C++ CONVERTER NOTE: Forward class declarations:
namespace bgu { namespace spl171 { namespace net { namespace impl { namespace TFTP { namespace msg { class Message; } } } } } }

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

					using bgu::spl171::net::api::MessageEncoderDecoder;
					using Message = bgu::spl171::net::impl::TFTP::msg::Message;

					/// <summary>
					/// Created by himelbrand on 1/9/17.
					/// </summary>
					class MessageEncoderDecoderImpl : public MessageEncoderDecoder<Message*>
					{

					private:
						std::vector<char> bytes(1 << 10); //start with 1k
						int len = 0;
						short opCode = 0;
						int datePacketSize = 0;

					public:
						virtual std::vector<char> encode(Message *message) override;

						virtual Message *decode(std::vector<char> &message);


					private:
						short bytesToShort(std::vector<char> &byteArr);

						std::vector<char> shortToBytes(short num);



					public:
						virtual Message *decodeNextByte(char nextByte) override;

					private:
						void pushByte(char nextByte);



					};

				}
			}
		}
	}
}
