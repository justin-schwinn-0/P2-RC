#ifndef MUTEX_RC_H
#define MUTEX_RC_H

#include <map>
#include <string>

#include "Node.h"
#include "Utils.h"

const int REQUEST = 1001;
const int GIVE = 1002;


class MutexRc
{
public:
    MutexRc(NodeInfo& ni);
    ~MutexRc();

    void handleMsg(std::string msg);

    void init();

    void request();
    void handleRequest(int uid,int ts);

    void tryEnterCs();

    std::string getCtrlStr(const int ctrlMsgId);

private:
    Node& rNode;

    std::map<int, bool> mKeys;

};

#endif

