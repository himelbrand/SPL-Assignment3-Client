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
						class Error : public Message
						{
						private:
							short errorCode = 0;
							std::wstring errorMsg;
						public:
							Error(short errorCode);

							virtual short getErrorCode();

							virtual std::wstring getErrorMsg();
						};

					}
				}
			}
		}
	}
}
