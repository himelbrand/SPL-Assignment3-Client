#include <stdlib.h>
#include <connectionHandler.h>
#include <queue>


using namespace std;
std::queue<std::string> lineQueue;



/**
* This function is running by a seconde thread who incharge off readding a new command from the client
 * and send it to the server after encoding
*/
void run(ConnectionHandler *connectionhandler) {
    std::cout << "< ";
    while (true) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        lineQueue.push(line);
		boost::mutex::scoped_lock f(ConnectionHandler::mtx);
        connectionhandler->sendLine(line);
		if(line=="DISC")
			break;
    }
}
/**
 * main function s running by the main thread who incharge off reading new arriavle messages from the sever, decode them,
 * and decides what to do next according to the arrival package
*/
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

    /**
     * starting the second thread
     */
    boost::thread th1(boost::bind(run, &connectionHandler));

    /**
    * keep decoding as long is still connected
    */
    while (!ConnectionHandler::disconnect) {
            connectionHandler.decode();
	}
	th1.join();
	return 0;
    }



