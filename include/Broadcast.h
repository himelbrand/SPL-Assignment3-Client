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
						class Broadcast : public Message
						{
						private:
							char isAdded = 0;
							std::wstring filename;
						public:
							Broadcast(char isAdded, const std::wstring &filename);

							virtual char getIsAdded();

							virtual std::wstring getFilename();
						};

					}
				}
			}
		}
	}
}
