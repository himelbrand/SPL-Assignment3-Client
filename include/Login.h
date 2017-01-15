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
						class Login : public Message
						{
						private:
							std::wstring username;
						public:
							Login(const std::wstring &username);
							virtual std::wstring getUsername();
						};

					}
				}
			}
		}
	}
}
