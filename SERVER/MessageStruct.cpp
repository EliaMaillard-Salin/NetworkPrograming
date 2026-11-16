#include "MessageStruct.h"

std::string MessageStruct::GetMessageBuffer(MessageStruct from)
{
    if (from.from.size() > 10)
        return "";

    char* type;
    memcpy(&type, &from.type, sizeof(MessageStruct::Type));
    char* name;
    memcpy(&name, &from.from, 10);
    
    
    std::string res;
    return std::string();
}

MessageStruct MessageStruct::GetMessageFrom(std::string from)
{
    return MessageStruct();
}
