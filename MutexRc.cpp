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
    Utils::printVectorPair(mKeys);
}

MutexRc::~MutexRc()
{
}

void MutexRc::handleMsg(std::string msg)
{

}

void MutexRc::init()
{
}

void MutexRc::request()
{
    for(auto it : mKeys)
    {
        if(it.second)
        {
            Utils::log("Have key",it.first);
        }
        else
        {
            Utils::log("Dont have Key",it.first);
        }
    }
}

std::string MutexRc::getCtrlStr(const std::string& ctrlMsgId)
{
    return std::to_string(rNode.getUid()) + APP_DELIM + ctrlMsgId;
}
