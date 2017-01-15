#pragma once

#include <string>
#include <vector>

//JAVA TO C++ CONVERTER NOTE: Forward class declarations:
namespace bgu { namespace spl171 { namespace net { namespace impl { namespace TFTP { namespace msg { class Message; } } } } } }
namespace bgu { namespace spl171 { namespace net { namespace impl { namespace TFTP { namespace msg { class Broadcast; } } } } } }

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

					using bgu::spl171::net::api::bidi::BidiMessagingProtocol;
					using bgu::spl171::net::api::bidi::Connections;
					using Message = bgu::spl171::net::impl::TFTP::msg::Message;
					using Broadcast = bgu::spl171::net::impl::TFTP::msg::Broadcast;


					class BidiMessagingProtocolImpl : public BidiMessagingProtocol<Message*>
					{

					private:
						int connectionId = 0;
						Connections<Message*> *connections;
						bool shouldClose = false;
					public:
						static ConcurrentHashMap<int, std::wstring> *const loggedInUsers;
					private:
						bool loggedIn = false;
						short lastOp = 0;
						File *file;
						std::vector<char> bytes;
						std::vector<char> blob(512);
						int blobLen = 0;
						FileInputStream *is = nullptr;
						FileOutputStream *os = nullptr;
						long long dataBlocksNeeded = 0;


					public:
						virtual ~BidiMessagingProtocolImpl()
						{
							delete connections;
							delete file;
							delete is;
							delete os;
						}

						virtual void start(int connectionId, Connections<Message*> *connections) override;

						virtual void process(Message *message) throw(FileNotFoundException) override;
					private:
						bool logIn(const std::wstring &userName);
						void logout();
						void broadcast(Broadcast *msg);

					public:
						virtual bool shouldTerminate() override;

					};

				}
			}
		}
	}
}
