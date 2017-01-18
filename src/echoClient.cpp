#include <stdlib.h>
#include <connectionHandler.h>
#include <queue>
#include <boost/thread.hpp>
//#include <Task.h>

using namespace std;
std::queue<std::string> lineQueue;
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/


std::mutex mtx;

void run(ConnectionHandler *connectionhandler) {
    std::cout << "< ";
    while (true) {
        const short bufsize = 1024;
        char buf[bufsize];

        std::cin.getline(buf, bufsize);
        std::string line(buf);
        lineQueue.push(line);
        std::cout << "< ";
        mtx.lock();
        connectionhandler->sendLine(line);
        mtx.unlock();
    }
}

int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }

    std::string host = argv[1];
    short port = atoi(argv[2]);


     ConnectionHandler connectionHandler(host, port);

    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        std::cerr << "Cannot connect to " << port << ":" << port << std::endl;
        return 1;
    }

    boost::thread th1(boost::bind(run, &connectionHandler));

    while (!ConnectionHandler::disconnect) {
            connectionHandler.decode(&mtx);
        }
    return 0;
    }



