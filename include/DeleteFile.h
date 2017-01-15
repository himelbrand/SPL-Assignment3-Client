#pragma once

#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/E22BB8BE-3870-4080-B45D-07B9A5C131FF/cpp/Message.h"
#include <string>

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
						class DeleteFile : public Message
						{
						private:
							std::wstring filename;
						public:
							DeleteFile(const std::wstring &filename);

							virtual std::wstring getFilename();
						};

					}
				}
			}
		}
	}
}
