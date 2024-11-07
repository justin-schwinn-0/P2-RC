#include "Node.h"
#include "Utils.h"

#include <iostream>
#include <climits>
#include <thread>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

Node::~Node()
{
    for(int fd : openRcv)
    {
        close(fd);
    }

    close(mListenFd);
}

void Node::openSocket()
{
    struct sockaddr_in serverSocket = {AF_INET,INADDR_ANY};
    serverSocket.sin_port = mListener.getPort();

    struct sctp_initmsg init;
    init.sinit_num_ostreams = 1;
    init.sinit_max_instreams = 1;
    init.sinit_max_attempts = 4;

    mListenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

    if(mListenFd < 0)
    {
        Utils::log( "couldn't make SCTP socket!" ); 
        return;
    }

    int ret = bind(mListenFd, (struct sockaddr*)&serverSocket,sizeof(serverSocket));
    if(ret < 0)
    {
        Utils::log( "coudn't bind socket: " , strerror(errno) );
        return;
    }

    ret = setsockopt(mListenFd, IPPROTO_SCTP, SCTP_INITMSG, &init,sizeof(init));
    if(ret < 0)
    {
        Utils::log( "coudn't set socket option INITMSG: " , strerror(errno) );
        return;
    }

    int flag = 1;
    ret = setsockopt(mListenFd, IPPROTO_SCTP, SCTP_NODELAY, &flag,sizeof(flag));
    if(ret < 0)
    {
        Utils::log( "coudn't set socket option NODELAY: " , strerror(errno) );
        return;
    }

    int maxBurst = 10;  
    ret = setsockopt(mListenFd, IPPROTO_SCTP, SCTP_MAX_BURST, &maxBurst,sizeof(maxBurst));
    if(ret < 0)
    {
        Utils::log( "coudn't set socket option NODELAY: " , strerror(errno) );
        return;
    }

    struct sctp_rtoinfo rtoinfo;
    rtoinfo.srto_min = 500;  // Set minimum RTO to 500 ms (adjust as needed)
    rtoinfo.srto_max = 3000; // Set maximum RTO to 3000 ms
    rtoinfo.srto_initial = 2000;  // Set initial RTO to 2000 ms
    
    ret = setsockopt(mListenFd, IPPROTO_SCTP, SCTP_RTOINFO, &rtoinfo, sizeof(rtoinfo));
    if(ret < 0)
    {
        Utils::log("coudn't set socket option RTOINFO: " , strerror(errno));
        return;
    }

    struct sctp_paddrparams paddrparams;
    memset(&paddrparams, 0, sizeof(paddrparams));
    paddrparams.spp_pathmaxrxt = 5;
    paddrparams.spp_hbinterval = 3000; 

    ret = setsockopt(mListenFd, IPPROTO_SCTP, SCTP_PEER_ADDR_PARAMS, &paddrparams, sizeof(paddrparams));
    if(ret < 0)
    {
        Utils::log("coudn't set socket option PEER_ADDR_PARAMS: " , strerror(errno));
        return;
    }

    ret = listen(mListenFd, init.sinit_max_instreams);
    if(ret < 0)
    {
        Utils::log( "coudn't listen!: " , strerror(errno) );
        return;
    }

    Utils::log( "socket open on port", mListener.getPort() );
}

void Node::acceptNeighbors()
{
    if(mListenFd < 0)
    {
        Utils::log( "bad listener!" , mListenFd );
        return;
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while(openRcv.size() < mNeighbors.size())
    {
        int rxFd = accept(mListenFd, (struct sockaddr*)&client_addr,&addr_len);

        if(rxFd < 0)
        {
            Utils::log( "couldn't accept connection: " , strerror(errno) );
            return;
        }

        openRcv.push_back(rxFd);
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        Utils::log("accepted connection:",client_ip);
    }

    Utils::log("accepted all neighbors");
}

void Node::sendTo(int uid, std::string msg)
{
    for(auto& c : mNeighbors)
    {
        if(c.getUid() == uid)
        {
            c.queueMessage(msg);
        }
    }
}

void Node::flood(std::string str)
{
    for(auto& c : mNeighbors)
    {
        c.queueMessage(str);
    }
}

void Node::sendExcept(int uid, std::string msg)
{
    for(auto& c : mNeighbors)
    {
        if(c.getUid() != uid)
        {
            c.queueMessage(msg);
        }
    }
}

void Node::releaseMessagesThread(int delay)
{
    while(!finishedAlg )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        releaseMessages();
    }
}

void Node::releaseMessages()
{
    for(auto& c : mNeighbors)
    {
        c.releaseMessages();
    }
}

void Node::listenToNeighbors()
{
    while(!finishedAlg )
    {
        for(int fd : openRcv)
        {
            if(Utils::pollForFd(fd,1,POLLIN) > 0)
            {
                recvMsg(fd); 
            }
        }
    }
}

void Node::recvMsg(int fd)
{
    const int bufSize = 1024;
    char buf[bufSize];

    struct sctp_sndrcvinfo sndrcvinfo;
    //sndrcvinfo.sinfo_stream = 1;
    int flags=0;
    
    int in = sctp_recvmsg(fd,buf,bufSize,NULL,0,&sndrcvinfo,&flags);

    if(in > 0)
    {
        std::string strMsg(buf);
        //Utils::log("                           got", strMsg);
        //std::cout << "             stream : " << sndrcvinfo.sinfo_stream << std::endl;
        //std::cout << "ssn : " << sndrcvinfo.sinfo_ssn << std::endl;
        //std::cout << "PPID: " << sndrcvinfo.sinfo_ppid << std::endl;
        //std::cout << "             Flags: " << flags << std::endl;
        auto splits = Utils::split(strMsg,MSG_DELIM);
        for(std::string str : splits)
        {
            msgHandler(str);
        }
    }
    else
    {
        Utils::log("something happened!",in,flags);
    }
}

Connection Node::getOwnConnection()
{
    return mListener;
}

int Node::getUid()
{
    return mUid;
}

void Node::print()
{
    std::cout << mUid << " ";
    mListener.print();
    std::cout << "\n\t";

    for(auto& c : mNeighbors)
    {
        c.print();
    }

    std::cout << std::endl;
}

void Node::connectAll()
{
    for(auto& n : mNeighbors)
    {
        n.makeConnection();
    }
}

void Node::addConnection(Connection c)
{
    mNeighbors.push_back(c);
}

bool Node::isLeader()
{
    return leader == mUid;
}

std::vector<int> Node::getConnectedUids()
{
    std::vector<int> uids;

    for(auto& n : mNeighbors)
    {
        uids.push_back(n.getUid());
    }

    return uids;
}
