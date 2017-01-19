#include <connectionHandler.h>
#include <boost/algorithm/string.hpp>
#include <stdio.h>

using boost::asio::ip::tcp;

using namespace std;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

boost::mutex ConnectionHandler::mtx;
bool ConnectionHandler::disconnect = false;
bool diconnectSend = false;


int count =0;
ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_),fs(),fsMode(),fileName(){}
    
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

    ConnectionHandler::mtx.try_lock(); //LOCK mtx start decoding
 //  std::cout << "start decode  -  opcode - " << std::to_string(op[1]) << std::endl;
    switch(op[1]) {
        case 3: // DATA DECODE
        {
            char blockNumberD[2];
            char packetSize[2];
            getBytes(packetSize, 2);
            getBytes(blockNumberD, 2);
            char dataBytes[bytesToShort(packetSize)];
            getBytes(dataBytes, (unsigned int)bytesToShort(packetSize));
            if (!fs.is_open()) { //DIRQ DECODE
                bool newWord = true;
                int i=0;
                for(char c:dataBytes){
                    if(c == 0) {
                        std::cout << "" << std::endl;
                        newWord = true;
                    }
                    else {
                        if(newWord) {
                            if(i ==0)
                            std::cout << "\n> ";
                            else
                                std::cout << "> ";
                            newWord = false;
                        }
                        std::cout << c;
                    }
                    i++;
                }
                std::cout << "< "<<std::flush;
              //  std::cout << "< ";
            } else { //RRQ DECODE
                try {
            //        std::cout << "data bytes size " << strlen(&dataBytes[0]) << "  " << bytesToShort(packetSize) << std::endl;
                    fs.write(dataBytes, bytesToShort(packetSize));
                } catch (int e) {
                    char errorMessage[4];
                    errorMessage[0] = 0;
                    errorMessage[1] = 5;
                    errorMessage[2] = 0;
                    errorMessage[3] = 2;
                    sendBytes(errorMessage, 4);
                    ConnectionHandler::mtx.unlock(); //UNLOCK MTX
                    break;

                }
                    if (bytesToShort(packetSize) < 512) {//TODO check databytes size , need to be 512 sometimes
                    fileName = "";
                        std::cout << "closing fs!" <<std::endl;
                    fs.close();


                }
            }
            char ackMessage[4];
            ackMessage[0] = 0;
            ackMessage[1] = 4;
            ackMessage[2] = blockNumberD[0];
            ackMessage[3] = blockNumberD[1];

            sendBytes(ackMessage, 4);
            if((unsigned int)bytesToShort(packetSize) < 512){
                ConnectionHandler::mtx.unlock(); //UNLOCK MTX
            }

            break;
    }
        case 4://ACK DECODE
        {
            if(diconnectSend){
                disconnect=true;
                std::cout <<"disconeeeeect" << std::endl;
                ConnectionHandler::mtx.unlock(); //UNLOCK MTX
            }
            char blockNumberA[2];
            char packetSize[2];
            getBytes(blockNumberA, 2);
            short bN = bytesToShort(blockNumberA);
            std::cout << "\n> ACK " + std::to_string(bN) << std::endl;
            std::cout << "< " <<std::flush;

            if (fs.is_open()) { //WRQ DECODE

                char dataBytesTemp[512];
                     char c;

                int dataSize =0;
                try {
                    while(dataSize < 512 && fs.get(c))
                    {
                        //std::cout  << " len : " << dataSize + 6  << " byte :  " << (char)c << std::endl;
                        dataBytesTemp[dataSize] = c;
                        dataSize++;

                    }


                } catch (int e) {
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
                shortToBytes((short)dataSize, packetSize);

                dataMessage[2] = packetSize[0];
                dataMessage[3] = packetSize[1];

                shortToBytes(bN, blockNumberA);

                dataMessage[4] = blockNumberA[0];
                dataMessage[5] = blockNumberA[1];


                for(int j=6; j<dataSize+6;j++){
                    dataMessage[j] = dataBytesTemp[j-6];
                }

              // std::cout << "> data  size " << dataSize     << std::endl;

                sendBytes(dataMessage, dataSize +6);
              //  cout << "send DATA " << endl;
                if (dataSize < 512) {
                 //   std::cout << "fs close!" <<std::endl;
                    fs.close();
                    ConnectionHandler::mtx.unlock(); //UNLOCK MTX
                }
            } else {
                ConnectionHandler::mtx.unlock(); //UNLOCK MTX
            }
    }
            break;
        case 5: {//ERROR DOCODE
            char errorCode[2];
            getBytes(errorCode, 2);
            std::cout << "\n> Error " + std::to_string(errorCode[1]) << std::endl;
            std::cout << "< " << std::flush;
            string errorMessage;

            if(fsMode == 'R') //Delete the local file
                std::remove(fileName.c_str());
            fs.close();
            fileName = "";
            ConnectionHandler::mtx.unlock(); //UNLOCK MTX
            return getFrameAscii(errorMessage, '\0');

        }
        case 9: {//Bcast
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
            std::cout << "\n> BCAST " << state << " " << fileADName << std::endl;
            std::cout << "< " << std::flush;
            ConnectionHandler::mtx.unlock(); //UNLOCK MTX
			break;
        }
        default: {
            ConnectionHandler::mtx.unlock(); //UNLOCK MTX
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
        std::cerr << "Not connected."<< std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const  char bytes[], int bytesToWrite) {

for(int i =0 ; i< bytesToWrite;i++){
  // cout <<"nextbyte: "<< to_string(bytes[i]) <<endl;
  //  cout <<"len: "<< i <<endl;
}
   // std::cout <<"opcode : " << std::to_string(bytes[1]) << std::endl;
    int tmp = 0;
   //std::cout << "byte to write: " << bytesToWrite << std::endl;

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
 


byteObj ConnectionHandler::encodeInput(std::string &message){
    std::vector<std::string> words;
    boost::split(words, message, boost::is_space());
	std::string command = words.at(0);
    fsMode = 'N';

  //  std::cout << "command : " +command << std::endl;
    if(command=="RRQ"){ //READ ENCODE
        if(words.size() == 2){
            char  * bytes = new char [words.at(1).length() +3];

            bytes[0] = 0;//one byte
            bytes[1] = 1;//another byte
            int i=2;
            for(char c:words.at(1)){
                bytes[i] = c;
                i++;
            }
            bytes[i] = '\0' ;

            fs.open(words.at(1));
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
            std::cout << "please enter one file name" << std::endl;
        }




	}else if(command=="WRQ"){
        if(words.size() == 2){
            char  * bytes = new char[words.at(1).length() +3];

            bytes[0] = 0;//one byte
            bytes[1] = 2;//another byte
            int i=2;
            for(char c:words.at(1)){
                bytes[i] = c;
                i++;
            }
            bytes[i] = '\0' ;



            fs.open(words.at(1),std::ios::in | std::ios::binary);



            if(!fs.good()){
                std::cout << "File not exist!!" << std::endl;
                fs.close();
                return byteObj();
            }
           // std::cout << " fs is inputstream open : " << fs.is_open() <<   fs.tellg()   <<std::endl;
            return byteObj(i + 1,bytes);
           // return bytes;
        }else{
            std::cout << "please add one file" << std::endl;
        }
	}else if(command =="LOGRQ"){
        if(words.size() == 2){

            char  * bytes = new char[9];


            bytes[0] = 0;
            bytes[1] = 7;

//            std::cout << std::to_string(bytess[0]) << std::endl;
//            std::cout << std::to_string(bytess[1]) << std::endl;
//
//
//            std::cout << strlen(&bytess[1]) << std::endl;
//            std::cout << sizeof(bytess) << std::endl;
//            std::cout << "$$$$$$$$$$$$$$$$$$$$" << std::endl;


//            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

           int i=2;
            for(char c:words.at(1)){
                bytes[i] = c;
                i++;
            }
            bytes[i] = '\0';


//            std::cout << strlen(&bytess[1]) << std::endl;
//            std::cout << "$$$$$$$$$$$$$$$$$$$$" << std::endl;
            return byteObj(i + 1,bytes);
          //  return bytess;
        }else{
            std::cout << "please enter username" << std::endl;
        }
	}else if(command=="DIRQ"){
        char  * bytes = new char[2];
        bytes[0] = 0;
        bytes[1] = 6;

        return byteObj(2,bytes);
       // return bytes;

	}else if(command=="DELRQ"){
        if(words.size() == 2){
            char  * bytes = new char[words.at(1).length() +3];

            bytes[0] = 0;//one byte
            bytes[1] = 8;//another byte
            int i=2;
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
     return byteObj();

}




bool ConnectionHandler::sendLine(std::string& line) {
    if(!line.empty()) {
        byteObj encodeMessage = encodeInput(line);


        if(encodeMessage._bytesArray != nullptr){


   //         std::cout << "send bytes size is " << encodeMessage._bytesArray[0] << std::endl;
            bool send =  sendBytes(encodeMessage._bytesArray, encodeMessage._bytesArraySize);

            return send;
        }else{
            std::cout << "*************not valid command" <<  std::endl;
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


