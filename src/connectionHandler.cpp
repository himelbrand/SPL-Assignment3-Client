#include <connectionHandler.h>
#include <boost/algorithm/string.hpp>

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;


bool ConnectionHandler::keepListen = false;


ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_),fs(),fileName(){}
    
ConnectionHandler::~ConnectionHandler() {
    close();
}

bool ConnectionHandler::connect() {

    std::cout << "Starting connect to " 
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\n');

}


bool ConnectionHandler::decode(){
    char op[2];

    if(!getBytes(op,2)){
        return false;
    }
    switch(op[1]) {
        case 3: // DATA
        {
            char blockNumberD[2];
            char packetSize[2];
            getBytes(packetSize, 2);
            getBytes(blockNumberD, 2);
            char dataBytes[bytesToShort(packetSize)];
            getBytes(dataBytes, sizeof(dataBytes));
            if (!fs.is_open()) {
                std::cout << dataBytes;

                if (sizeof(dataBytes) < 512) {//TODO check databytes size , need to be 512 sometimes
                    keepListen = false;
                }
            } else {
                try {
                    fs.write(dataBytes, sizeof(dataBytes));
                } catch (int e) {
                    char errorMessage[4];
                    errorMessage[0] = 0;
                    errorMessage[1] = 5;
                    errorMessage[2] = 0;
                    errorMessage[3] = 2;
                    sendBytes(errorMessage, 4);
                    break;
                }
                if (sizeof(dataBytes) < 512) {//TODO check databytes size , need to be 512 sometimes
                    fileName = "";
                    fs.close();
                    keepListen = false;
                }
            }
            char ackMessage[4];
            ackMessage[0] = 0;
            ackMessage[1] = 4;
            ackMessage[2] = blockNumberD[0];
            ackMessage[3] = blockNumberD[1];

            sendBytes(ackMessage, 4);

            break;
    }
        case 4://ACK
        {
            char blockNumberA[2];
            getBytes(blockNumberA, 2);
            short bN = bytesToShort(blockNumberA);
            std::cout << "> ACK " + bN << std::endl;
            if (fs.is_open()) {
                char *dataBytes= nullptr;
                try {
                    fs.readsome(dataBytes, 512);
                } catch (int e) {
                    char errorMessage[4];
                    errorMessage[0] = 0;
                    errorMessage[1] = 5;
                    errorMessage[2] = 0;
                    errorMessage[3] = 2;
                    sendBytes(errorMessage, 4);
                    break;
                }


                char dataMessage[sizeof(dataBytes) + 6];
                dataMessage[0] = 0;
                dataMessage[1] = 3;
                bN++;

                shortToBytes(bN, blockNumberA);
                dataMessage[2] = blockNumberA[0];
                dataMessage[3] = blockNumberA[1];

                shortToBytes(sizeof(dataBytes), blockNumberA);

                dataMessage[4] = blockNumberA[0];
                dataMessage[5] = blockNumberA[1];

                std::strcat(dataMessage, dataBytes);
                sendBytes(dataMessage, sizeof(dataMessage));
                if (sizeof(dataBytes) < 512) {
                    fs.close();
                    keepListen = false;
                }
            } else {
                keepListen = false;
                std::cout << " fs close " << std::endl;
            }
    }
            break;
        case 5: {//Error
            char errorCode[2];
            getBytes(errorCode, 2);
            std::cout << "> Error " + errorCode[1] << std::endl;
            string errorMessage;
            keepListen = false;
            return getFrameAscii(errorMessage, '\0');
        }
        case 9: {//Bcast
            keepListen = false;
			break;
        }
        default: {
            keepListen = false;//TODO: check if needed
            return false;
            break;
        }
    }
	return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);			
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 


char*  ConnectionHandler::encodeInput(std::string &message){
    std::vector<std::string> words;
    boost::split(words, message, boost::is_space());
	std::string command = words.at(0);
    char* ans = nullptr;

    if(command=="RRQ"){
        if(words.size() != 2){
            char  bytes[words.at(1).length() +3];

            bytes[0] = 0;//one byte
            bytes[1] = 1;//another byte
            int i=2;
            for(char c:words.at(1)){
                bytes[i] = c;
                i++;
            }
            bytes[i] = '\0' ;
            ans = bytes;

            fs.open(words.at(1));
           if(fs){
               std::cout << "The file is already exist!!" << std::endl;
               return nullptr;
           }else{
               fileName = words.at(1);
           }
        }else{
            std::cout << "please enter username" << std::endl;
        }


	}else if(command=="WRQ"){
        if(words.size() != 2){
            char  bytes[words.at(1).length() +3];

            bytes[0] = 0;//one byte
            bytes[1] = 2;//another byte
            int i=2;
            for(char c:words.at(1)){
                bytes[i] = c;
                i++;
            }
            bytes[i] = '\0' ;
            ans = bytes;

            fs.open(words.at(1));
            if(!fs.good()){
                std::cout << "File not exist!!" << std::endl;
                return nullptr;
            }


        }else{
            std::cout << "please enter username" << std::endl;
        }
	}else if(command =="LOGRQ"){
        if(words.size() != 2){
            char  bytes[words.at(1).length() +3];

            bytes[0] = 0;//one byte
            bytes[1] = 7;//another byte
           int i=2;
            for(char c:words.at(1)){
                bytes[i] = c;
                i++;
            }
            bytes[i] = '\0' ;
            ans = bytes;

        }else{
            std::cout << "please enter username" << std::endl;
        }
	}else if(command=="DIRQ"){
        char  bytes[2];
        bytes[0] = 0;
        bytes[1] = 6;

        ans = bytes;

	}else if(command=="DELRQ"){
        if(words.size() != 2){
            char  bytes[words.at(1).length() +3];

            bytes[0] = 0;//one byte
            bytes[1] = 8;//another byte
            int i=2;
            for(char c:words.at(1)){
                bytes[i] = c;
                i++;
            }
            bytes[i] = '\0' ;
            ans = bytes;
        }

	}else if(command=="DISC"){
        char  bytes[2];
        bytes[0] = 0;
        bytes[1] = 10;
        ans = bytes;
	}else{//not a valid command
        return nullptr;
	}
    return ans;
}




bool ConnectionHandler::sendLine(std::string& line) {

    if(!line.empty()) {
        char * encodeMessage = encodeInput(line);
        if(encodeMessage != nullptr){
            return sendBytes(encodeMessage, sizeof(encodeMessage));;
        }else{
            return false;
        }
    } else{
        return false;
    }

}
 
bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
		do{
			getBytes(&ch, 1);
            frame.append(1, ch);
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
//bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
//	bool result=sendBytes(frame.c_str(),frame.length());
//	if(!result) return false;
//	return sendBytes(&delimiter,1);
//}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try {
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}
    void ConnectionHandler::shortToBytes(short num, char* bytesArr)
    {
        bytesArr[0] = ((num >> 8) & 0xFF);
        bytesArr[1] = (num & 0xFF);
    }

    short ConnectionHandler::bytesToShort(char* bytesArr)
    {
        short result = (short)((bytesArr[0] & 0xff) << 8);
        result += (short)(bytesArr[1] & 0xff);
        return result;
    }
