#pragma once
#include <string>

struct MessageStruct
{
	enum Type
	{
		MESSAGE,
		LOG,
	};
	Type type;
	std::string from;
	std::string content;

	static int const FROM_SIZE_MAX = 10;

	std::string GetMessageBuffer(MessageStruct from);
	static MessageStruct GetMessageFrom( std::string from);
};

