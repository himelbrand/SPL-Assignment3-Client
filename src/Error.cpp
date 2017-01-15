#include "Error.h"

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

						Error::Error(short errorCode) : Message((short) 5)
						{
							//TODO: change error messages
							this->errorCode = errorCode;
							switch (errorCode)
							{
								case 0:
									this->errorMsg = L"undefined Error.";
									break;
								case 1:
									this->errorMsg = L"Requested File not found, could not complete operation.";
									break;
								case 2:
									this->errorMsg = L"Access violation – File cannot be written, read or deleted.";
									break;
								case 3:
									this->errorMsg = L"Disk full or allocation exceeded – No room in disk.";
									break;
								case 4:
									this->errorMsg = L"Illegal TFTP operation – Unknown Opcode.";
									break;
								case 5:
									this->errorMsg = L"File already exists – File name exists on WRQ.";
									break;
								case 6:
									this->errorMsg = L"User not logged in – Any opcode received before Login completes.";
									break;
								case 7:
									this->errorMsg = L"User already logged in – Login username already connected.";
									break;
							}
							this->packetSize = 5 + errorMsg.getBytes()->length;
						}

						short Error::getErrorCode()
						{
							return errorCode;
						}

						std::wstring Error::getErrorMsg()
						{
							return errorMsg;
						}
					}
				}
			}
		}
	}
}
