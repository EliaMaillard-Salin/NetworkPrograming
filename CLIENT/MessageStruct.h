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
};

