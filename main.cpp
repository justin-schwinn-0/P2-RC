#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <functional>
#include <climits>

#include "Node.h"
#include "Connection.h"
#include "Utils.h"
#include "MutexRc.h"

NodeInfo readConfig(std::string configFile, int popId = -1)
{
    std::ifstream file(configFile) ;
    Utils::log( "reading file: " , configFile );

    std::string wholeFile;
    std::string line;

    while(std::getline(file,line))
    {
        wholeFile += line + "\n";
    }
    auto lines = Utils::split(wholeFile,"\n");



    // sanitize lines to remove comments
    
    for(auto it = lines.rbegin(); it != lines.rend(); it++)
    {
        // erase the while line if it starts with #
        std::string& str = *it;
        if(str.at(0) == '#')
        {
            lines.erase((it+1).base());
        }
        else
        {
           size_t pos = 0;
           // if we find a #, erase it and everything after
           if((pos = str.find("#")) != std::string::npos)
           {
               str.erase(str.begin()+pos,str.end());
           }
        }
    }
    // use lines to create N nodes, and return 1 for this process
    auto firstLine = Utils::split(lines[0]," ");

    int numNodes;
    int interRequestDelay;
    int execTime;
    int totalRequests;
    if(firstLine.size() != 4)
    {
        // if length of the first line isn't 1,
        // then something when wrong in the sanitization 
        // or the config is invalid
        Utils::log( "First valid line of Config is wrong length!" );
    }
    else
    {
        numNodes = Utils::strToInt(firstLine[0]);
        interRequestDelay = Utils::strToInt(firstLine[1]);
        execTime          = Utils::strToInt(firstLine[2]);
        totalRequests     = Utils::strToInt(firstLine[3]);
    }


    std::vector<Node> nodes;
    for(int i = 1; i < numNodes+1;i++)
    {
        auto splitNode = Utils::split(lines[i]," ");

        if(splitNode.size() == 3)
        {
            std::istringstream uidSS(splitNode[0]); 
            std::istringstream portSS(splitNode[2]); 

            int uid;
            uint32_t port;

            uidSS >> uid;
            portSS >> port;

            Node n(uid,{uid,splitNode[1],port});
            nodes.push_back(n);
            //Utils::log( "adding node " , uid );

        }
        else
        {
            Utils::log( "node Line " , i , " is invalid" );
        }
        
    }


    for(auto& n : nodes)
    {
        for(auto& m : nodes)
        {
            if(n.getUid() != m.getUid())
            {
                n.addConnection(m.getOwnConnection());
            }
        }
    }

    /*for(auto& n : nodes)
    {
        n.addConnection(
            {n.getUid(),
             n.getOwnConnection().getHostname(),
             n.getOwnConnection().getPort(),
             INT_MAX});
    }*/

    /*for(auto& n : nodes)
    {
        n.print();
    }*/
    if(popId != -1)
    {
        for(auto n : nodes)
        {
            if(n.getUid() == popId)
            {
                return {interRequestDelay,
                        execTime,
                        totalRequests,
                        n,
                        configFile};
            }
        }   

    }

    Utils::log("returning node 0");
    return {interRequestDelay,
            execTime,
            totalRequests,
            nodes[0],
            configFile};
           
}

void runAlg(NodeInfo& ni)
{
    ni.n.print();
    ni.n.openSocket();

    MutexRc mrc(ni);

    ni.n.setHandler(std::bind(&MutexRc::handleMsg,mrc,std::placeholders::_1));

    std::thread connectThrd(&Node::connectAll, &ni.n);

    ni.n.acceptNeighbors();
    connectThrd.join();

    if(ni.n.getUid() == 0)
    {
        mrc.init();
    }

    mrc.request();

    std::thread releaseThrd(&Node::releaseMessagesThread, &ni.n,250);
    releaseThrd.detach();

    ni.n.listenToNeighbors();
}

int main(int argc,char** argv)
{
    if(argc == 3)
    {
        int uid = std::stoi(argv[2]);

        auto nodeData = readConfig(argv[1],uid);
        nodeData.configName = argv[1];

        runAlg(nodeData);
    }
    else
    {
        readConfig("asyncConfig.txt");
    }
    return 0;
}
