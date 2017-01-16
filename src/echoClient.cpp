#include <stdlib.h>
#include <connectionHandler.h>
#include <queue>
#include <boost/thread.hpp>
//#include <Task.h>

static bool disconnect = false;
std::queue<std::string> lineQueue;
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
void run(){
	while(true) {
		const short bufsize = 1024;
		char buf[bufsize];
		std::cout << "< ";
		std::cin.getline(buf, bufsize);
		std::string line(buf);
		lineQueue.push(line);
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
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }
	
	//From here we will see the rest of the ehco client implementation:

    boost::mutex mutex;
   // Task task1(1, &mutex);
    boost::thread th1(run);


    while (!disconnect) {
        if(/*!Task::*/!lineQueue.empty()) {
            std::string newLine = /*Task::*/lineQueue.front();
            lineQueue.pop();

            connectionHandler.keepListen = connectionHandler.sendLine(newLine);

            while(connectionHandler.keepListen){
                std::cout << "***************enter decode" << std::endl;
                connectionHandler.decode();
                std::cout << "*****************exit decode" << std::endl;
            }

        }





//        if (!connectionHandler.sendLine(newLine)) {
//            std::cout << "Disconnected. Exiting...\n" << std::endl;
//            break;
//        }

            // connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
          //  std::cout << "Sent " << len + 1 << " bytes to server" << std::endl;


            // We can use one of three options to read data from the server:
            // 1. Read a fixed number of characters
            // 2. Read a line (up to the newline character using the getline() buffered reader
            // 3. Read up to the null character

            //std::string answer;
//
//           char answer[1024];
//
//            // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
//            // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
//            if (!connectionHandler.getBytes(answer,2)) {
//                std::cout << "Disconnected. Exiting...\n" << std::endl;
//                break;
//            }
//
//
//
//
//            len = answer.length();
//            // A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
//            // we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
//            answer.resize(len - 1);
//            std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
//            if (answer == "bye") {
//                std::cout << "Exiting...\n" << std::endl;
//                break;
//            }
        }
    return 0;
    }



