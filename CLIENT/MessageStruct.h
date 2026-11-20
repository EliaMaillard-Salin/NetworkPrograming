#pragma once
#include <string>

struct MessageStruct
{
	enum Type
	{
		LOG,
		MESSAGE,
		GAME_INFO,
		FRAME_UPDATE,
	};
	Type type;
	std::string from;
	std::string content;

	static MessageStruct GetFromBuffer(char* buffer);
	std::string CreateBufferFromMessage();
};

