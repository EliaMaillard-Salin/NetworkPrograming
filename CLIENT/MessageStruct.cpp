#include "MessageStruct.h"

MessageStruct MessageStruct::GetFromBuffer(char* buffer)
{
	MessageStruct out;
	std::string in = buffer;
	out.type = static_cast<MessageStruct::Type>(in[0] - '0');
	in = in.erase(0,1);
	out.from = in.substr(0, 10);
	out.content = in.erase(0, 10);
	return out;
}

std::string MessageStruct::CreateBufferFromMessage()
{
	std::string out;
	out += std::to_string(type);
	from.resize(10, ' ');
	out += from;
	out += content;
	return out;
}
