#ifndef MUTEX_RC_H
#define MUTEX_RC_H

#include <map>
#include <string>

#include "Node.h"
#include "Utils.h"


class MutexRc
{
public:
    MutexRc(NodeInfo& ni);
    ~MutexRc();

    void handleMsg(std::string msg);


    void init();

    std::string getCtrlStr(int ctrlMsgId);

private:
    Node& rNode;

};

#endif

