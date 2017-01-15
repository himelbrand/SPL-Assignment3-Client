#pragma once

#include "Message.h"
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
						class ReadWrite : public Message
						{
						private:
							std::wstring filename;

						public:
							ReadWrite(short opCode, const std::wstring &filename);
							virtual std::wstring getFilename();
						};

					}
				}
			}
		}
	}
}
