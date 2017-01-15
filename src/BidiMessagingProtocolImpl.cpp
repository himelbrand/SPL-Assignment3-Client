#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/BidiMessagingProtocolImpl.h"
#include "Message.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/DataMessage.h"
#include "ReadWrite.h"
#include "Error.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/Acknowledge.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/Broadcast.h"
#include "Login.h"
#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/EED0158E-6473-41BB-BBA3-BAE939304AA2/cpp/DeleteFile.h"

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
					using DataMessage = bgu::spl171::net::impl::TFTP::msg::DataMessage;
					using Message = bgu::spl171::net::impl::TFTP::msg::Message;
					using DeleteFile = bgu::spl171::net::impl::TFTP::msg::DeleteFile;
					using Login = bgu::spl171::net::impl::TFTP::msg::Login;
					using ReadWrite = bgu::spl171::net::impl::TFTP::msg::ReadWrite;
					using Acknowledge = bgu::spl171::net::impl::TFTP::msg::Acknowledge;
					using Broadcast = bgu::spl171::net::impl::TFTP::msg::Broadcast;
					using Error = bgu::spl171::net::impl::TFTP::msg::Error;
java::util::concurrent::ConcurrentHashMap<int, std::wstring> *const BidiMessagingProtocolImpl::loggedInUsers = new java::util::concurrent::ConcurrentHashMap<int, std::wstring>();

					void BidiMessagingProtocolImpl::start(int connectionId, Connections<Message*> *connections)
					{
						this->connectionId = connectionId;
						this->connections = connections;
					}

					void BidiMessagingProtocolImpl::process(Message *message) throw(FileNotFoundException)
					{

						std::wstring filesList;
						std::vector<std::wstring> names;
						short blockNum;
						DataMessage *newData;
						short currentOpcode = message->getOpCode();


						if (loggedIn || currentOpcode == 7)
						{

							switch (currentOpcode)
							{
								case 1: //RRQ
									lastOp = 1;
									file = new File(std::wstring(L"Files/") + (static_cast<ReadWrite*>(message))->getFilename());
									if (file->exists())
									{
										dataBlocksNeeded = file->length() / 512 + 1;
										is = new FileInputStream(file);
										try
										{
											if ((blobLen = is->read(blob)) != -1)
											{
												DataMessage tempVar(static_cast<short>(blobLen),static_cast<short>(1),Arrays::copyOfRange(blob,0,blobLen));
												connections->send(connectionId,&tempVar);
											}
										}
										catch (const IOException &e)
										{ //TODO:check when this happens
											e->printStackTrace();
										}
									}
									else
									{
										Error tempVar2(static_cast<short>(1));
										connections->send(connectionId, &tempVar2); //file not found
									}
									break;
								case 2: //WRQ
									lastOp = 2;
									file = new File(std::wstring(L"Files/") + (static_cast<ReadWrite*>(message))->getFilename());
									if (file->exists())
									{ //ERROR
										Error tempVar3(static_cast<short>(5));
										connections->send(connectionId, &tempVar3); //file already exists
									}
									else
									{
										file = new File(std::wstring(L"Temp/") + (static_cast<ReadWrite*>(message))->getFilename());
										try
										{
											if (!file->exists())
											{
												file->createNewFile();
												os = new FileOutputStream(file);
												Acknowledge tempVar4(static_cast<short>(0));
												connections->send(connectionId, &tempVar4);
											}
											else
											{
												Error tempVar5(static_cast<short>(5));
												connections->send(connectionId, &tempVar5); //file already exists
											}
										}
										catch (const IOException &e)
										{
											e->printStackTrace();
										}
									}
									break;
								case 3: //DATA from client
								{
									short dataSize = (static_cast<DataMessage*>(message))->getDataSize();
									std::vector<char> data = (static_cast<DataMessage*>(message))->getData();
									blockNum = (static_cast<DataMessage*>(message))->getBlockNum();
									try
									{
										os->write(data);
										os->flush();
										Acknowledge tempVar6(blockNum);
										connections->send(connectionId,&tempVar6);
									}
									catch (const IOException &e)
									{ //TODO: send error msg , probably
										e->printStackTrace();
									}
//JAVA TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to the exception 'finally' clause:
									finally
									{
										try
										{
											if (dataSize < 512)
											{
												File tempVar7(std::wstring(L"Files/") + file->getName());
												Files::move(file->toPath(),(&tempVar7)->toPath());
												Broadcast tempVar8(static_cast<char>(1),file->getName());
												broadcast(&tempVar8);
												delete os;
											}
										}
										catch (const IOException &e)
										{
											e->printStackTrace();
										}

									}
									break;
								}
								case 4: // ACK
									blockNum = (static_cast<Acknowledge*>(message))->getBlockNum();
									switch (lastOp)
									{
										case 1: //RRQ
											try
											{
												if ((blobLen = is->read(blob)) != -1)
												{
													std::vector<char> tempBlob = Arrays::copyOfRange(blob, 0, blobLen);
													DataMessage tempVar9(static_cast<short>(tempBlob.size()), static_cast<short>(blockNum + 1), tempBlob);
													connections->send(connectionId, &tempVar9);
												}
												else
												{
													if (file->length() % 512 == 0)
													{
														DataMessage tempVar10(static_cast<short>(0), static_cast<short>(blockNum + 1), new char[0]);
														connections->send(connectionId, &tempVar10);
													}
													is = nullptr;
												}
											}
											catch (const IOException &e)
											{ //TODO:check when this happens
												e->printStackTrace();
											}
											break;
										case 6: //DIRQ
										//                            file = new File("/Files/");
										//                            filesList = "";
										//                            names = file.list() == null ? new ArrayList<>(): new ArrayList<>(Arrays.asList(file.list()));
										//                            for (String fileName : names) {
										//                                filesList += fileName + "\0";
										//                            }
										//                            bytes = filesList.getBytes();
										//                            if (blockNum < dataBlocksNeeded)
										//                                if (bytes.length == blockNum * 512) {
										//                                    newData = new DataMessage((short) 0, (short) (blockNum + 1), new byte[0]);
										//                                    connections.send(connectionId, newData);
										//                                } else if (bytes.length > (int) blockNum * 512) {
										//                                    int packetSize = (bytes.length < 512 * (blockNum + 1) ? bytes.length - (512 * blockNum) : 512);
										//                                    newData = new DataMessage((short) packetSize, (short) (blockNum + 1), Arrays.copyOfRange(bytes, 512 * blockNum, (bytes.length < 512 * (blockNum + 1) ? bytes.length : 512 * (blockNum + 1))));
										//                                    connections.send(connectionId, newData);
										//                                }
											if (blockNum < dataBlocksNeeded)
											{

												try
												{
													if ((blobLen = is->read(blob)) != -1)
													{
														std::vector<char> tempBlob = Arrays::copyOfRange(blob, 0, blobLen);
														DataMessage tempVar11(static_cast<short>(tempBlob.size()), static_cast<short>(blockNum + 1), tempBlob);
														connections->send(connectionId, &tempVar11);
													}
													else
													{
														if (file->length() % 512 == 0)
														{
															DataMessage tempVar12(static_cast<short>(0), static_cast<short>(blockNum + 1), new char[0]);
															connections->send(connectionId, &tempVar12);
														}
														is = nullptr;
													}
												}
												catch (const IOException &e)
												{
													e->printStackTrace();
												}

											}
											else
											{
												file->delete();
												is = nullptr;
												os = nullptr;
												file = nullptr;
											}
											break;

									}
									break;
								case 5: //Error - upload/download failed in client side
									if (lastOp == 2)
									{
										try
										{
											delete os;
											if (file->exists())
											{
												file->delete();
											}
										}
										catch (const IOException &e)
										{
											e->printStackTrace();
										}
									}
									file = nullptr;
									os = nullptr;
									is = nullptr;
									break;
								case 6: //DIRQ Packets

									file = new File(std::wstring(L"Temp/") + connectionId + std::wstring(L"DIR"));
									os = new FileOutputStream(file);
									lastOp = 6;

									for (auto name : (new File(L"Files/"))->list())
									{
										try
										{
											os->write(name.getBytes());
											os->write(static_cast<char>(L'\0'));
										}
										catch (const IOException &e)
										{
											e->printStackTrace();
										}
									}
									is = new FileInputStream(file);
									bytes = std::vector<char>(file->length() > 512 ? 512 : static_cast<int>(file->length()));

									try
									{
										is->read(bytes);
									}
									catch (const IOException &e)
									{
										e->printStackTrace();
									}
									dataBlocksNeeded = file->length() / 512 + 1;

									newData = new DataMessage(static_cast<short>(6), static_cast<short>(0), bytes);
									connections->send(connectionId, newData);

										//
										//                    names = new ArrayList<>(Arrays.asList(file.list()));
										//                    for (String fileName : names) {
										//                        filesList += fileName + "\0";
										//                    }
										//                    bytes = filesList.getBytes();
										//                    dataBlocksNeeded = bytes.length / 512 + 1;
										//                    newData = new DataMessage((short) (6), (short) 0, Arrays.copyOfRange(bytes, 0, (bytes.length < 512 ? bytes.length : 512)));
										//                    connections.send(connectionId, newData);

									break;
								case 7: //Login
									if (logIn((static_cast<Login*>(message))->getUsername()))
									{
										Acknowledge tempVar13(static_cast<short>(0));
										connections->send(connectionId, &tempVar13);
									}
									else
									{
										Error tempVar14(static_cast<short>(7));
										connections->send(connectionId, &tempVar14); //User already logged in
									}
									break;
								case 8:
									file = new File(std::wstring(L"Files/") + (static_cast<DeleteFile*>(message))->getFilename());
									if (file->exists())
									{
										file->delete();
										Acknowledge tempVar15(static_cast<short>(0));
										connections->send(connectionId, &tempVar15); //file deleted
										Broadcast tempVar16(static_cast<char>(0),(static_cast<DeleteFile*>(message))->getFilename());
										broadcast(&tempVar16);
									}
									else
									{
										Error tempVar17(static_cast<short>(1));
										connections->send(connectionId, &tempVar17); //User already logged in
									}
									break;
								case 10:
									Acknowledge tempVar18(static_cast<short>(0));
									connections->send(connectionId, &tempVar18); //user disconnected
									logout();
									connections->disconnect(connectionId);
									break;
								default:
									Error tempVar19(static_cast<short>(4));
									connections->send(connectionId, &tempVar19); //unknown opcode
									break;
							}
						}
						else
						{
							if (currentOpcode < 1 || currentOpcode>10)
							{
								Error tempVar20(static_cast<short>(4));
								connections->send(connectionId, &tempVar20); //unknown opcode
							}
							else
							{
								Error tempVar21(static_cast<short>(6));
								connections->send(connectionId, &tempVar21); //User not logged in
							}
						}
					}

					bool BidiMessagingProtocolImpl::logIn(const std::wstring &userName)
					{
//JAVA TO C++ CONVERTER TODO TASK: Multithread locking is not converted to native C++:
						synchronized(loggedInUsers)
						{
							if (loggedInUsers->containsValue(userName))
							{
								return false;
							}
							else
							{
								loggedInUsers->put(connectionId,userName);
								loggedIn = true;
								return true;
							}
						}
					}

					void BidiMessagingProtocolImpl::logout()
					{
//JAVA TO C++ CONVERTER TODO TASK: Multithread locking is not converted to native C++:
						synchronized(loggedInUsers)
						{
							loggedInUsers->remove(connectionId);
							loggedIn = false;
							shouldClose = true;
						}
					}

					void BidiMessagingProtocolImpl::broadcast(Broadcast *msg)
					{
						for (* : :optional<int> id : loggedInUsers->keySet())
						{
							connections->send(id,msg);
						}
					}

					bool BidiMessagingProtocolImpl::shouldTerminate()
					{
						return shouldClose;
					}
				}
			}
		}
	}
}
