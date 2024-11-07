#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>
#include <string>
#include <mutex>

#include <sys/socket.h>
#include <sys/types.h>

class Connection
{    
public:
    Connection(int uid ,std::string h, uint32_t p); 

    Connection(); 
    ~Connection();

    int getUid()
    { return mUid; }

    void sendMsgNow(std::string msg);

    void queueMessage(std::string msg);

    void releaseMessages();

    void makeConnection();

    uint32_t getPort()
    { return port; }

    const std::string& getHostname()
    { return hostname; }

    void print();
private:
    // hostname/ip of connection
    std::string hostname;
    uint32_t port;

    int mCon;

    int mUid;

    std::vector<std::string> mQueuedMessages;
};

#endif
