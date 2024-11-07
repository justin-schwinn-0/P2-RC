#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>
#include <functional>

#include "Connection.h"

class Node
{
public:
    Node(int uid, Connection con):
        mUid(uid),
        mListener(con),
        finishedAlg(false)
    {
    }

    ~Node();

    void openSocket();

    void acceptNeighbors();

    Connection getOwnConnection();
    int getUid();

    void flood(std::string str);

    void sendTo(int uid, std::string msg);
    void sendExcept(int uid, std::string msg);

    void releaseMessagesThread(int delay);
    void releaseMessages();

    void print();
    void addConnection(Connection c);

    void setHandler(std::function<void(std::string)> fun)
    { msgHandler = fun;}

    int getNeighborsSize()
    { return mNeighbors.size(); }

    void connectAll();

    void listenToNeighbors();

    void recvMsg(int fd);

    void finishAlg()
    { finishedAlg = true; }

    void resetAlg()
    { finishedAlg = false; }

    void setIsLeader(int l)
    { leader = l; }

    bool isLeader();

    std::vector<int> getConnectedUids();

private:
    int mUid;
    Connection mListener;
    std::vector<Connection> mNeighbors;

    int mListenFd;
    
    std::function<void(std::string)> msgHandler;

    std::vector<int> openRcv;

    bool finishedAlg;

    int leader;
};

struct NodeInfo
{
    int interRequestDelay;
    int execTime;
    int totalRequests;
    Node n;
    std::string configName;
};
#endif
