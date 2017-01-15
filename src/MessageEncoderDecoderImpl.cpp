#include "MessageEncoderDecoderImpl.h"
#include "Message.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/Broadcast.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/Acknowledge.h"
#include "Error.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/DataMessage.h"
#include "ReadWrite.h"
#include "Login.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/DeleteFile.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/DirRequest.h"
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
					using bgu::spl171::net::api::MessageEncoderDecoder;
					using DataMessage = bgu::spl171::net::impl::TFTP::msg::DataMessage;
					using Message = bgu::spl171::net::impl::TFTP::msg::Message;
					using DeleteFile = bgu::spl171::net::impl::TFTP::msg::DeleteFile;
					using Login = bgu::spl171::net::impl::TFTP::msg::Login;
					using ReadWrite = bgu::spl171::net::impl::TFTP::msg::ReadWrite;
					using DirRequest = bgu::spl171::net::impl::TFTP::msg::DirRequest;
					using Disconnect = bgu::spl171::net::impl::TFTP::msg::Disconnect;
					using Acknowledge = bgu::spl171::net::impl::TFTP::msg::Acknowledge;
					using Broadcast = bgu::spl171::net::impl::TFTP::msg::Broadcast;
					using Error = bgu::spl171::net::impl::TFTP::msg::Error;

					std::vector<char> MessageEncoderDecoderImpl::encode(Message *message)
					{

						std::vector<char> encodedMessage(message->getPacketSize());
						std::vector<char> tempBytes;
						encodedMessage[0] = shortToBytes(message->getOpCode())[0];
						encodedMessage[1] = shortToBytes(message->getOpCode())[1];
						switch (message->getOpCode())
						{
							case 9: //broadcast
								encodedMessage[2] = (static_cast<Broadcast*>(message))->getIsAdded();
								tempBytes = (static_cast<Broadcast*>(message))->getFilename().getBytes();
								for (int i = 0; i < tempBytes.size() ;i++)
								{
									encodedMessage[i + 3] = tempBytes[i];
								}
								encodedMessage[encodedMessage.size() - 1] = 0;

								if ((static_cast<Broadcast*>(message))->getFilename().find(L"/0") != std::wstring::npos)
								{
									encodedMessage.clear();
								}
								break;
							case 4:
								encodedMessage[2] = shortToBytes((static_cast<Acknowledge*>(message))->getBlockNum())[0];
								encodedMessage[3] = shortToBytes((static_cast<Acknowledge*>(message))->getBlockNum())[1];
								break;
							case 5:
								encodedMessage[2] = shortToBytes((static_cast<Error*>(message))->getErrorCode())[0];
								encodedMessage[3] = shortToBytes((static_cast<Error*>(message))->getErrorCode())[1];
								tempBytes = (static_cast<Error*>(message))->getErrorMsg().getBytes();
								for (int i = 0; i < tempBytes.size() ;i++)
								{
									encodedMessage[i + 4] = tempBytes[i];
								}
								encodedMessage[encodedMessage.size() - 1] = 0;

								if ((static_cast<Error*>(message))->getErrorMsg().find(L"/0") != std::wstring::npos)
								{
									encodedMessage.clear();
								}
								break;
							case 3:
								encodedMessage[2] = shortToBytes((static_cast<DataMessage*>(message))->getDataSize())[0];
								encodedMessage[3] = shortToBytes((static_cast<DataMessage*>(message))->getDataSize())[1];
								encodedMessage[4] = shortToBytes((static_cast<DataMessage*>(message))->getBlockNum())[0];
								encodedMessage[5] = shortToBytes((static_cast<DataMessage*>(message))->getBlockNum())[1];
								for (int i = 0; i < (static_cast<DataMessage*>(message))->getData().size() ;i++)
								{
									encodedMessage[i + 6] = (static_cast<DataMessage*>(message))->getData()[i];
								}
								break;
						}
						return encodedMessage;
					}

					Message *MessageEncoderDecoderImpl::decode(std::vector<char> &message)
					{
						Message *decodeMessage;
						std::vector<char> tempArray;
						short opCode = bytesToShort(message);
						switch (opCode)
						{
							case 1:
							case 2:
							{
								tempArray = Arrays::copyOfRange(message, 2,message.size() - 1);
								std::wstring fileName = std::wstring(tempArray, StandardCharsets::UTF_8);
								decodeMessage = new ReadWrite(opCode,fileName);
								break;
							}
							case 3:
							{
								tempArray = Arrays::copyOfRange(message, 6,message.size());
								short dataSize = bytesToShort(Arrays::copyOfRange(tempArray,2,4));
								short blockNum = bytesToShort(Arrays::copyOfRange(tempArray,4,6));
								decodeMessage = new DataMessage(dataSize,blockNum,tempArray);

								datePacketSize = 0;
								break;
							}
							case 4:
								decodeMessage = new Acknowledge(bytesToShort(Arrays::copyOfRange(message,2,4)));
								break;
							case 7:
							{
								tempArray = Arrays::copyOfRange(message, 2,message.size());
								std::wstring username = std::wstring(tempArray, StandardCharsets::UTF_8);
								decodeMessage = new Login(username);
								break;

							}
							case 8:
							{
								 tempArray = Arrays::copyOfRange(message, 2,message.size() - 1);
								std::wstring deleteFileName = std::wstring(tempArray, StandardCharsets::UTF_8);
								decodeMessage = new DeleteFile(deleteFileName);
								break;
							}
							case 6:
								decodeMessage = new DirRequest();
								break;
							case 10:
								decodeMessage = new Disconnect();
								break;
							default:
								decodeMessage = new Error(static_cast<short>(4));
								break;

						}
						len = 0;
						return decodeMessage;
					}

					short MessageEncoderDecoderImpl::bytesToShort(std::vector<char> &byteArr)
					{
						short result = static_cast<short>((byteArr[0] & 0xff) << 8);
						result += static_cast<short>(byteArr[1] & 0xff);
						return result;
					}

					std::vector<char> MessageEncoderDecoderImpl::shortToBytes(short num)
					{
						std::vector<char> bytesArr(2);
						bytesArr[0] = static_cast<char>((num >> 8) & 0xFF);
						bytesArr[1] = static_cast<char>(num & 0xFF);
						return bytesArr;
					}

					Message *MessageEncoderDecoderImpl::decodeNextByte(char nextByte)
					{
						//notice that the top 128 ascii characters have the same representation as their utf-8 counterparts
						//this allow us to do the following comparison

						if (len == 2)
						{
							opCode = bytesToShort(bytes);
						}
						if (opCode != 0)
						{
							switch (opCode)
							{
								case 1:
								case 2:
								case 7:
								case 8:
									if (nextByte == L'\0')
									{
										return decode(bytes);
									}
									break;
								case 3:
									if (len == 4)
									{
										datePacketSize = bytesToShort(Arrays::copyOfRange(bytes, 2, 4)) + 6;
									}
									if (datePacketSize == len)
									{
										return decode(bytes);
									}
									break;
								case 4:
									if (len == 4)
									{
										return decode(bytes);
									}
									break;
								case 6:
								case 10:
									return decode(bytes);
								default:
									return decode(bytes);
							}
						}
						pushByte(nextByte);
						return nullptr; //not a line yet
					}

					void MessageEncoderDecoderImpl::pushByte(char nextByte)
					{
						if (len >= bytes.size())
						{
							bytes = Arrays::copyOf(bytes, len * 2);
						}

						bytes[len++] = nextByte;
					}
				}
			}
		}
	}
}
