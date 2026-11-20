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

	static MessageStruct GetFromBuffer(char* buffer);
	std::string CreateBufferFromMessage();
};

