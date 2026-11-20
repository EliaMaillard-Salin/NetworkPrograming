#include "MessageStruct.h"


MessageStruct MessageStruct::GetFromBuffer(char* buffer)
{
	MessageStruct out;
	std::string in = buffer;
	out.type = static_cast<MessageStruct::Type>(in[0] - '0');
	in = in.erase(0,1);
	out.title = in.substr(0, 10);
	out.content = in.erase(0, 10);
	return out;
}

std::string MessageStruct::CreateBufferFromMessage()
{
	std::string out;
	out += std::to_string(type);
	title.resize(10, ' ');
	out += title;
	out += content;
	return out;
}

std::string DuckData::ConvertToString()
{
    std::string out;
    out += std::to_string(duckID);
    out += "#";
    out += std::to_string(DuckDist);
    out += "#";
    out += std::to_string(duckSpriteID);
    out += "#";
    out += std::to_string(duckSpriteState);
    out += "#";
    out += duckName;
    out += "#";
    return out;
}

DuckData DuckData::GetFromString(std::string& in)
{
    DuckData out;
    std::string duckIDStr = in.substr(0, in.find("#"));
    out.duckID = std::stoi(duckIDStr);
    in = in.erase(0, in.find("#") + 1);
    std::string duckDistStr = in.substr(0, in.find("#"));
    out.DuckDist = std::stoi(duckDistStr);
    in = in.erase(0, in.find("#") + 1);
    std::string duckSpriteIDStr = in.substr(0, in.find("#"));
    out.duckSpriteID = std::stoi(duckSpriteIDStr);
    in = in.erase(0, in.find("#") + 1);
    std::string duckSpriteStateStr = in.substr(0, in.find("#"));
    out.duckSpriteState = std::stoi(duckSpriteStateStr);
    in = in.erase(0, in.find("#") + 1);
    out.duckName = in.substr(0, in.find("#"));
    in = in.erase(0, in.find("#") + 1);

    return out;
}
