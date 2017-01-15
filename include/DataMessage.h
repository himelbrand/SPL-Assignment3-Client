#pragma once

#include "../../../../Library/Containers/com.apple.mail/Data/Library/Mail Downloads/E22BB8BE-3870-4080-B45D-07B9A5C131FF/cpp/Message.h"
#include <vector>

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
						class DataMessage : public Message
						{
						private:
							short dataSize = 0;
							short blockNum = 0;
							std::vector<char> data;
						public:
							DataMessage(short dataSize, short blockNum, std::vector<char> &data);
							virtual short getDataSize();
							virtual short getBlockNum();
							virtual std::vector<char> getData();
						};

					}
				}
			}
		}
	}
}
