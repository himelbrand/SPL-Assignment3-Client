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
					namespace msg
					{

						DeleteFile::DeleteFile(const std::wstring &filename) : Message((short) 8)
						{
							this->filename = filename;
							this->packetSize = 3 + filename.getBytes()->length;
						}

						std::wstring DeleteFile::getFilename()
						{
							return filename;
						}
					}
				}
			}
		}
	}
}
