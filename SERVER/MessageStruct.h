#pragma once
#include <string>

struct DuckData
{
	int duckID = -1;
	int DuckDist;
	int duckSpriteID;
	bool duckSpriteState;
	std::string duckName;

	std::string ConvertToString();
	static DuckData GetFromString(std::string& in);

};

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
	std::string title;
	std::string content;

	static int const FROM_SIZE_MAX = 10;

	static MessageStruct GetFromBuffer(char* buffer);
	std::string CreateBufferFromMessage();
};

