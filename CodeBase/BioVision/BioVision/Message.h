#ifndef MESSAGE_H
#define MESSAGE_H

class LogManager;

#include <string>

using namespace std;

class Message
{
public:
    Message();
    Message(int cmdCode, string flag, string requestOrigin);

    int getCmdCode();
    void setCmdCode(int cmdCode);

    string getFlag();
    void setFlag(string flag);

    string getRequestOrigin();
    void setRequestOrigin(string requestOrigin);

    string getBriefDescription();
    void setBriefDescription(string briefDescription);

    string getDetailedDescription();
    void setDetailedDescription(string detailedDescription);

private:
    int _cmdCode;
    string _flag;
    string _requestOrigin;
    string _briefDescription;
    string _detailedDescription;
};

#endif // MESSAGE_H
