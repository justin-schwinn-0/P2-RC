#ifndef MUTEX_RC_H
#define MUTEX_RC_H

#include <map>
#include <string>
#include <climits>

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

    void giveKey(int uid);
    void handleGive(int uid, int ts);

    void tryEnterCs();

    void releaseKeys();

    bool hasRequest()
    {   return mRequestTime == INT_MAX; }

    std::string getCtrlStr(const int ctrlMsgId);

private:
    Node& rNode;

    std::map<int, bool> mKeys;

    int mTime = 0;

    int mRequestTime = INT_MAX;

    std::map<int, bool> mOtherRequests;

};

#endif

