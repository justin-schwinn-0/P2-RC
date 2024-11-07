#include "Connection.h"
#include "Utils.h"

#include <iostream>
#include <thread>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

Connection::Connection(int uid,std::string h, uint32_t p) : 
    mUid(uid),
    hostname(h),
    port(p)
{
}

Connection::~Connection()
{
    close(mCon);
}

void Connection::sendMsgNow(std::string msg)
{
    if(mCon < 0)
    {
        Utils::log("we aren't connected to ",hostname);
        return;
    }
    bool sent = false;
    do
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int ret = sctp_sendmsg(mCon,(void *)msg.c_str(),strlen(msg.c_str())+1,NULL,0,0,0,0,10,0);

        if(ret < 0)
        {
            Utils::error("send failed");
            Utils::log("send faied with ",ret,mCon );
        }
        else
        {
            sent = true;
            //Utils::log("                         sent:" ,msg, "to ", mUid);
        }
    }
    while(!sent);
}

void Connection::makeConnection()
{

    std::string addr = Utils::getAddressFromHost(hostname);

    struct sockaddr_in serverAddress;
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = port;
    serverAddress.sin_addr.s_addr = inet_addr(addr.c_str());

    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if(sd < 0)
    {
        Utils::log( "couldn't make SCTP socket!" ); 
        return;
    }


    int ret;
    do
    {
        sleep(2);
        ret = connect(sd,(struct sockaddr*)&serverAddress,sizeof(serverAddress));

        if(ret < 0)
        {
            Utils::error("connect failed "+ hostname + " " + std::to_string(port));
        }

    }
    while(ret != 0 );

    mCon = sd;
    Utils::log("connection with",hostname,"fd:",sd);
}

void Connection::queueMessage(std::string msg)
{
    mQueuedMessages.push_back(msg);
}

void Connection::releaseMessages()
{
    std::string msg = "";
    if(mQueuedMessages.size() > 1)
    {
        for(int i = 0; i < mQueuedMessages.size(); i++)
        {
            msg += mQueuedMessages[i] + (i+1 < mQueuedMessages.size() ? MSG_DELIM : "");
        }
    }
    else if(mQueuedMessages.size() == 1)
    {
        msg = mQueuedMessages[0];
    }
    else
    {
        // no messages to release!
        return;
    }

    mQueuedMessages.clear();

    sendMsgNow(msg);
}

void Connection::print()
{
    std::cout << "{ " << hostname << " "<< mUid <<" " << port << " }";
}
