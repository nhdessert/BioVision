class LogManager;

#include "Message.h"

Message::Message()
{
}

Message::Message(int cmdCode, string flag, string requestOrigin)
{
    _cmdCode = cmdCode;
    _flag = flag;
    _requestOrigin = requestOrigin;

    //TODO instantiate LogManger
}

int Message::getCmdCode()
{
    return _cmdCode;
}

void Message::setCmdCode(int cmdCode)
{
    _cmdCode = cmdCode;
}

string Message::getFlag()
{
    return _flag;
}

void Message::setFlag(string flag)
{
    _flag = flag;
}

string Message::getRequestOrigin()
{
    return _requestOrigin;
}

void Message::setRequestOrigin(string requestOrigin)
{
    _requestOrigin = requestOrigin;
}

string Message::getBriefDescription()
{
    return _briefDescription;
}

void Message::setBriefDescription(string briefDescription)
{
    _briefDescription = briefDescription;
}

string Message::getDetailedDescription()
{
    return _detailedDescription;
}

void Message::setDetailedDescription(string detailedDescription)
{
    _detailedDescription = detailedDescription;
}
