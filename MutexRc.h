#ifndef MUTEX_RC_H
#define MUTEX_RC_H

#include <map>
#include <string>

#include "Node.h"
#include "Utils.h"

const std::string REQUEST = "REQ";
const std::string GIVE = "GIVE";


class MutexRc
{
public:
    MutexRc(NodeInfo& ni);
    ~MutexRc();

    void handleMsg(std::string msg);

    void init();

    std::string getCtrlStr(const std::string& ctrlMsgId);

private:
    Node& rNode;

    std::map<int, bool> mKeys;

};

#endif

