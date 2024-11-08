#include "MutexRc.h"

MutexRc::MutexRc(NodeInfo& ni):
    rNode(ni.n)
{

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

std::string MutexRc::getCtrlStr(int ctrlMsgId)
{
    return std::to_string(rNode.getUid()) + APP_DELIM + std::to_string(ctrlMsgId);
}
