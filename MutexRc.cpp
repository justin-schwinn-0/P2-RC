#include "MutexRc.h"

MutexRc::MutexRc(NodeInfo& ni):
    rNode(ni.n)
{
    auto neighbors = rNode.getConnectedUids();

    for(int uid : neighbors)
    {
        //Lower UID gets the key
        mKeys[uid] = rNode.getUid() < uid;
    }
}

MutexRc::~MutexRc()
{
}

void MutexRc::handleMsg(std::string msg)
{

}

void MutexRc::init()
{
    request();
}

void MutexRc::request()
{
    Utils::printVectorPair(mKeys);
    bool needsKeys = false;
    for(auto it : mKeys)
    {
        if(!it.second)
        {
            Utils::log("will ask for key",it.first);
        }
    }
}

std::string MutexRc::getCtrlStr(const std::string& ctrlMsgId)
{
    return std::to_string(rNode.getUid()) + APP_DELIM + ctrlMsgId;
}
