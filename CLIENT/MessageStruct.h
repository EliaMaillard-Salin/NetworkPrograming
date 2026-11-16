#pragma once
#include <string>

struct MessageStruct
{
	enum Type
	{
		MESSAGE,
		LOG,
	};
	std::string from;
	std::string content;
};

