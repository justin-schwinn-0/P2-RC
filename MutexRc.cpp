#include "MutexRc.h"

#include <thread>

MutexRc::MutexRc(NodeInfo& ni):
    rNi(ni),
    rNode(ni.n)
{
    auto neighbors = rNode.getConnectedUids();

    for(int uid : neighbors)
    {
        //Lower UID gets the key
        mKeys[uid] = rNode.getUid() < uid;
        mOtherRequests[uid] = false;
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

        mTime = std::max(mTime+1,timeStamp);

        switch(msgId)
        {
            case REQUEST:
                handleRequest(uid,timeStamp);
                break;
            case GIVE:
                handleGive(uid,timeStamp);
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
    //request();
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
    mTime++;
    mTotalRequests++;
}

void MutexRc::giveKey(int uid)
{
    mKeys[uid] = false;
    rNode.sendTo(uid,getCtrlStr(GIVE));
}

void MutexRc::handleRequest(int uid,int ts)
{
    if(!hasRequest())
    {
        giveKey(uid);
        return;
    }

    //Utils::log("handle request");
    
    if(ts < mRequestTime)
    {
        //Utils::log("Other Request has priority");
    }
    else if( ts == mRequestTime && uid < rNode.getUid() )
    {
        Utils::log("deferMyKey");
    }
    else
    {
        //Utils::log("keep my key");
    }
}

void MutexRc::handleGive(int uid, int ts)
{
    //Utils::log("was given key!", uid);

    mKeys[uid] = true;

    tryEnterCs();
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
        releaseKeys();
    }
}

void MutexRc::releaseKeys()
{
    Utils::log("Gives keys to who needs it");
    mRequestTime = INT_MAX;
    requestTimer();
}

void MutexRc::requestTimer()
{
    auto timerLambda = [&]()
    {   
        std::this_thread::sleep_for(std::chrono::milliseconds(rNi.interRequestDelay));
        request();
    };

    if(mTotalRequests < rNi.totalRequests)
    {
        std::thread timerThrd(timerLambda);
        timerThrd.detach();
    }
}

std::string MutexRc::getCtrlStr(const int ctrlMsgId)
{
    return std::to_string(rNode.getUid()) + APP_DELIM + std::to_string(ctrlMsgId) + APP_DELIM + std::to_string(mTime);
}
