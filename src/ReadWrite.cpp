#include "ReadWrite.h"

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

						ReadWrite::ReadWrite(short opCode, const std::wstring &filename) : Message(opCode)
						{
							this->filename = filename;
							this->packetSize = 3 + filename.getBytes()->length;
						}

						std::wstring ReadWrite::getFilename()
						{
							return filename;
						}
					}
				}
			}
		}
	}
}
