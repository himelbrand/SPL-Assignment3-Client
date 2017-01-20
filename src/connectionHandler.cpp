#include <connectionHandler.h>
#include <boost/algorithm/string.hpp>


using boost::asio::ip::tcp;
using namespace std;

boost::mutex ConnectionHandler::mtx;
bool ConnectionHandler::disconnect = false;

//true is the user sent a DISC request
bool diconnectSend = false;

//Constructor
ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_),fs(),fsMode(),fileName(){}
//Destructor
ConnectionHandler::~ConnectionHandler() {
	close();
}

//Connection function, connect to the host
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

//The connectionHandler decode function,decode incoming packets from the server
bool ConnectionHandler::decode(){
	char op[2];

	if(!getBytes(op,2)){
		return false;
	}
	boost::mutex::scoped_lock f(ConnectionHandler::mtx);

	switch(op[1]) {
		case 3: // DATA packet decode
		{
			char blockNumberD[2];
			char packetSize[2];
			getBytes(packetSize, 2);
			getBytes(blockNumberD, 2);
			char dataBytes[bytesToShort(packetSize)];
			getBytes(dataBytes, (unsigned int)bytesToShort(packetSize));

            //DIRQ decode , prints each one of the files on the server
			if (!fs.is_open()) {
				bool newWord = true;
				int i=0;
				for(char c:dataBytes){
					if(c == 0) {
						std::cout << "" << std::endl;
						newWord = true;
					}
					else {
						if(newWord) {
							std::cout << "> ";
							newWord = false;
						}
						std::cout << c;
					}
					i++;
				}
				std::cout << "< "<<std::flush;

			} else { //RRQ decode
				try {
					fs.write(dataBytes, bytesToShort(packetSize));
				} catch (int e) { //In case of an error message, access violation – File cannot be written, read or deleted.
					char errorMessage[4];
					errorMessage[0] = 0;
					errorMessage[1] = 5;
					errorMessage[2] = 0;
					errorMessage[3] = 2;
					sendBytes(errorMessage, 4);
					break;

				}
                //Means the last data packet as arrived
				if (bytesToShort(packetSize) < 512) {
					std::cout <<"> RRQ " << fileName << " complete" <<std::endl;
					std::cout << "< "<<std::flush;
					fileName = "";
					fs.close();


				}
			}
            //send ack message as response for the arrived data packet
			char ackMessage[4];
			ackMessage[0] = 0;
			ackMessage[1] = 4;
			ackMessage[2] = blockNumberD[0];
			ackMessage[3] = blockNumberD[1];

			sendBytes(ackMessage, 4);
			if((unsigned int)bytesToShort(packetSize) < 512){
			}
			break;
		}
		case 4://ACK decode
		{
            //In case the client sent a DISC packet
			if (diconnectSend) {
				disconnect = true;
				std::cout << "> ACK 0" << std::endl;
			} else{
				char blockNumberA[2];
				char packetSize[2];
				getBytes(blockNumberA, 2);
				short bN = bytesToShort(blockNumberA);
				std::cout << "> ACK " + std::to_string(bN) << std::endl;

				if (fs.is_open()) { //WRQ decode
					fsMode = 'W';
					char dataBytesTemp[512];
					char c;

					int dataSize = 0;
					try {
						while (dataSize < 512 && fs.get(c)) {
							dataBytesTemp[dataSize] = c;
							dataSize++;

						}


					} catch (int e) { //In case of an error message
						char errorMessage[4];
						errorMessage[0] = 0;
						errorMessage[1] = 5;
						errorMessage[2] = 0;
						errorMessage[3] = 2;
						sendBytes(errorMessage, 4);
						break;
					}


					char dataMessage[dataSize + 6];
					dataMessage[0] = 0;
					dataMessage[1] = 3;
					bN++;
					shortToBytes((short) dataSize, packetSize);

					dataMessage[2] = packetSize[0];
					dataMessage[3] = packetSize[1];

					shortToBytes(bN, blockNumberA);

					dataMessage[4] = blockNumberA[0];
					dataMessage[5] = blockNumberA[1];

                    //Fill the data in the data packet
					for (int j = 6; j < dataSize + 6; j++) {
						dataMessage[j] = dataBytesTemp[j - 6];
					}

                    //In case this is the last data packet needs to be sent
					sendBytes(dataMessage, dataSize + 6);
					if (dataSize < 512) {
						fs.close();
					}
				} else {
					if(fsMode != 'W')
					std::cout << "< " << std::flush;
					else{
						fsMode = 'N';
                        std::cout <<"> WRQ " << fileName << " complete" <<std::endl;
                        fileName = "";
                        }
				}
			}
		}
			break;
		case 5: {//ERROR deocde
			char errorCode[2];
			getBytes(errorCode, 2);
			std::cout << "> Error " + std::to_string(errorCode[1]) << std::endl;
			std::cout << "< " << std::flush;
			string errorMessage;

			if(fsMode == 'R') //Delete the local file
				std::remove(fileName.c_str());
			fs.close();
			fileName = "";
			return getFrameAscii(errorMessage, '\0');

		}
		case 9: {//BACST Decode
			char added[1];
			getBytes(added, 1);
			string state;
			if(added[0] == 0)
			{
				state = "del";
			}else{
				state = "add";
			}
			string fileADName;
			getFrameAscii(fileADName, '\0');
			std::cout << "> BCAST " << state << " " << fileADName << std::endl;
			std::cout << "< " << std::flush;
			break;
		}
		default: {
			return false;
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
		std::cerr << "Not connected."<< std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::sendBytes(const  char bytes[], int bytesToWrite) {
	int tmp = 0;

	boost::system::error_code error;
	try {
		while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
		}
		if(error)
			throw boost::system::system_error(error);
	} catch (std::exception& e) {
		std::cerr << "Not connected.."<< std::endl;
		return false;
	}
	return true;
}


//Encoding function, encoding the client commands into server packets
byteObj ConnectionHandler::encodeInput(std::string &message){
	std::vector<std::string> words;
	boost::split(words, message, boost::is_space());
	std::string command = words.at(0);
	fsMode = 'N';

	if(command=="RRQ"){ //READ ENCODE
        //Check if the client command is legal
		if(words.size() == 2){
			char  * bytes = new char [words.at(1).length() +3];

			bytes[0] = 0;
			bytes[1] = 1;
			int i=2;
            //Fill in the file name inside the RRQ packet
			for(char c:words.at(1)){
				bytes[i] = c;
				i++;
			}
			bytes[i] = '\0' ;

			fs.open(words.at(1));
            //Checks if the file is already exist on the client side
			if(fs){
				std::cout << "The file is already exist!!" << std::endl;
				return byteObj();
			}else{
				fs.open(words.at(1),std::ios::out);
				fsMode = 'R';
				fileName = words.at(1);
			}
			return byteObj(i +1,bytes);
		}else{
			std::cout << "< *please enter one file name" << std::endl;
            std::cout << "< " << std::flush;
		}




	}else if(command=="WRQ"){
        //Check if the client command is legal
		if(words.size() == 2){
			char  * bytes = new char[words.at(1).length() +3];

			bytes[0] = 0;
			bytes[1] = 2;
			int i=2;
            //Fill in the file name inside the WRQ packet
			for(char c:words.at(1)){
				bytes[i] = c;
				i++;
			}

			bytes[i] = '\0' ;

			fs.open(words.at(1),std::ios::in | std::ios::binary);
			if(!fs.good()){
				std::cout << "< *File not exist!!" << std::endl;
				std::cout << "< " << std::flush;
				fs.close();
				return byteObj();
			}
            fileName = words.at(1);
			return byteObj(i + 1,bytes);
		}else{
			std::cout << "< *Please add one file" << std::endl;
			std::cout << "< " << std::flush;
		}
	}else if(command =="LOGRQ"){
        //Check if the client command is legal
		if(words.size() == 2){
			char  * bytes = new char[9];
			bytes[0] = 0;
			bytes[1] = 7;
			int i=2;
            //Fill in the username inside the LOGRQ packet
			for(char c:words.at(1)){
				bytes[i] = c;
				i++;
			}
			bytes[i] = '\0';
			return byteObj(i + 1,bytes);
		}else{
			std::cout << "< *Please enter a valid username" << std::endl;
            std::cout << "< " << std::flush;
            return  byteObj();
		}
	}else if(command=="DIRQ"){
		char  * bytes = new char[2];
		bytes[0] = 0;
		bytes[1] = 6;

		return byteObj(2,bytes);

	}else if(command=="DELRQ"){
        //Check if the client command is legal
		if(words.size() == 2){
			char  * bytes = new char[words.at(1).length() +3];

			bytes[0] = 0;//one byte
			bytes[1] = 8;//another byte
			int i=2;
            //Fill in the username inside the DELRQ packet
			for(char c:words.at(1)){
				bytes[i] = c;
				i++;
			}
			bytes[i] = '\0' ;
			return byteObj(i + 1,bytes);
		}

	}else if(command=="DISC"){
		char * bytes = new char[2];
		bytes[0] = 0;
		bytes[1] = 10;
		diconnectSend = true;
		return byteObj(2,bytes);
	}
    std::cout << "< *not valid command" <<  std::endl;
    std::cout << "< " <<  std::flush;
	return byteObj();

}




bool ConnectionHandler::sendLine(std::string& line) {
	if(!line.empty()) {
		byteObj encodeMessage(encodeInput(line));
		if(encodeMessage._bytesArray != nullptr){
			bool send =  sendBytes(encodeMessage._bytesArray, encodeMessage._bytesArraySize);
			return send;
		}else{
			return false;
		}
	} else{
		std::cout << "< " <<  std::flush;
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
		std::cerr << "Not connected..."<< std::endl;
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


