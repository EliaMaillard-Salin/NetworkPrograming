#pragma once
#include <string>

struct MessageStruct
{
	enum Type
	{
		LOG,
		MESSAGE,
		INPUT,
	};
	Type type;
	std::string from;
	std::string content;

	static int const FROM_SIZE_MAX = 10;

	static MessageStruct GetFromBuffer(char* buffer);
	std::string CreateBufferFromMessage();
};

