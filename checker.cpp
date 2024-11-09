#include <fstream>
#include <climits>
#include <map>
#include "Utils.h"

void checkFile(std::string path)
{
    std::ifstream file;
    file.open(path);

    int currentNodeinCs = INT_MAX;
    std::string line;
    int lineNum = 0;
    bool hasFailed = false;

    std::map<int, int> totalRequests;
    std::map<int, uint64_t> beginTimes;
    std::map<int, uint64_t> endTimes;


    while(std::getline(file,line))
    {
    	auto segments = Utils::split(line," ");
	int nodeInLine = Utils::strToInt(segments[1]);
	if(segments[0].compare("ENTER") == 0)
	{
	    if(currentNodeinCs == INT_MAX)
	    {
		currentNodeinCs = nodeInLine;
		if(segments.size() == 3)
		{
		    beginTimes[nodeInLine] = Utils::strToULong(segments[2]);
		}
		//Utils::log("Process",nodeInLine,"enters");
	    }
	    else
	    {
		Utils::log("Mutex Fails at ENTER line:", lineNum, line);
		hasFailed = true;
	    }
	}
	else if(segments[0].compare("EXIT") == 0)
	{
	    if(currentNodeinCs == nodeInLine)
	    {
		currentNodeinCs = INT_MAX;
		totalRequests[nodeInLine] += 1;
		if(segments.size() == 3)
		{
		    endTimes[nodeInLine] = Utils::strToULong(segments[2]);
		}
		//Utils::log("Process",nodeInLine,"exits");
	    }
	    else
	    {
		Utils::log("Mutex Fails at EXIT line:", lineNum, line);
		hasFailed = true;
	    }
	}
	lineNum++;
    }

    if(hasFailed)
    {
	Utils::log("Mutex Alg Failed!");
    }
    else
    {
    	Utils::log("Mutex Passed!");
	Utils::log("File contains the following stats:");
	Utils::log("");
	Utils::log("{nodeId, CS enterances}");
	Utils::printVectorPair(totalRequests);
	Utils::log("");

	double totalRate;
	for(auto it : beginTimes)
	{
	    double time =(endTimes[it.first] - it.second) / 1000.0;
	    double rate = totalRequests[it.first] / time;
	    Utils::log("Time spent by node ", it.first,":",std::to_string(time)+"s");
	    Utils::log("CS per second:",std::to_string(rate)+"s");
	    Utils::log("");
	    totalRate += rate;
	    
	}


	Utils::log("Total rate:",totalRate);
    }
}

int main(int argc,char** argv)
{
    if(argc == 2)
    {
    	checkFile(argv[1]);
    }
    else
    {
        Utils::log("must have 1 parameter, the path to the file to check");
    }
    return 0;
}

