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
    Utils::log("got message:",msg);
    auto splits = Utils::split(msg, APP_DELIM);

    if(splits.size() == 3)
    {
        int uid = Utils::strToInt(splits[0]);
        int msgId = Utils::strToInt(splits[1]); 
        int timeStamp = Utils::strToInt(splits[2]); 

        switch(msgId)
        {
            case REQUEST:
                handleRequest(uid,timeStamp);
                break;
            default:
                Utils::log("Unknown message!",msgId);
                break;
        }
    }
    else
    {
        Utils::log("something went wrong with the message");
    }
}

void MutexRc::init()
{
    request();
}

void MutexRc::request()
{
    Utils::printVectorPair(mKeys);
    bool needsKeys = false;
    mRequestTime = mTime;
    for(auto it : mKeys)
    {
        if(!it.second)
        {
            Utils::log("will ask for key",it.first);
            needsKeys=true;
            rNode.sendTo(it.first,getCtrlStr(REQUEST));
        }
    }

    if(!needsKeys)
    {
        tryEnterCs();
    }
}
void MutexRc::handleRequest(int uid,int ts)
{
    Utils::log("handling request");
    
    if(ts < mRequestTime)
    {
        Utils::log("Other Request has priority");
    }
    else if( ts == mRequestTime && uid < rNode.getUid())
    {
        Utils::log("Will defer");
    }
}

void MutexRc::tryEnterCs()
{
    bool canEnter = true;
    for(auto it : mKeys)
    {
        if(!it.second)
        {
            canEnter = false;
        }
    }

    if(canEnter)
    {
        Utils::log("ENTER CS");
        //wait time
        Utils::log("EXIT CS");
        //release keys
        Utils::log("should release keys");
    }
}

void releaseKeys()
{
    Utils::log("Gives keys to who needs it");
}

std::string MutexRc::getCtrlStr(const int ctrlMsgId)
{
    return std::to_string(rNode.getUid()) + APP_DELIM + std::to_string(ctrlMsgId) + APP_DELIM + std::to_string(mTime);
}
