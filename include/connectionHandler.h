#ifndef CONNECTION_HANDLER__
#define CONNECTION_HANDLER__

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <fstream>

using namespace std;
using boost::asio::ip::tcp;

/**
 * This class represents a packet message ,
 * it's fields are  char array and the size of the char array
 */
class byteObj{
public:
	//Constructor
    byteObj (int bytesArraySize, char * bytesArray) : _bytesArraySize(bytesArraySize), _bytesArray(bytesArray) {}
	//Destructor
    virtual ~byteObj (){
        delete[] this->_bytesArray;
        this->_bytesArraySize = 0;
    }
	//Assignment operator
	byteObj& operator=(const byteObj &other){
		this->_bytesArray = other._bytesArray;
		this->_bytesArraySize = other._bytesArraySize;
		return *this;
	}
	//Copy constructor
	byteObj(const byteObj &other):_bytesArraySize(other._bytesArraySize),_bytesArray(other._bytesArray){}
	//Constructor for null pointer bytes array
    byteObj () : _bytesArraySize(0), _bytesArray(nullptr) {}

	//represents the size of the packet message
    int _bytesArraySize;
	//represents the packet message
    char * _bytesArray;
};//class byteObj


class ConnectionHandler {
private:
	const std::string host_;
	const short port_;
	boost::asio::io_service io_service_;   // Provides core I/O functionality
	tcp::socket socket_;

    std::fstream fs;
	//describe the transaction that occurring right now
	//W - write, R-read, N- not read and not write
    char fsMode;
	//The name of the file in use in case of write/read transaction
    std::string fileName;

    short bytesToShort(char* bytesArr);
    void shortToBytes(short num, char* bytesArr);



public:

    ConnectionHandler(std::string host, short port);
    virtual ~ConnectionHandler();

	//tells if the user decided to disconnect or not
    static bool disconnect;
	//used for synchronized between the two threads
    static boost::mutex mtx;
	//represents a packet message send to the server
    byteObj encodeInput(std::string & message);
    bool decode();
    // Connect to the remote machine
    bool connect();

    // Read a fixed number of bytes from the server - blocking.
    // Returns false in case the connection is closed before bytesToRead bytes can be read.
    bool getBytes(char bytes[], unsigned int bytesToRead);

	// Send a fixed number of bytes from the client - blocking.
    // Returns false in case the connection is closed before all the data is sent.
    bool sendBytes(const char bytes[], int bytesToWrite);

    // Read an ascii line from the server
    // Returns false in case connection closed before a newline can be read.
    bool getLine(std::string& line);

	// Send an ascii line from the server
    // Returns false in case connection closed before all the data is sent.
    bool sendLine(std::string& line);

    // Get Ascii data from the server until the delimiter character
    // Returns false in case connection closed before null can be read.
    bool getFrameAscii(std::string& frame, char delimiter);

    // Send a message to the remote host.
    // Returns false in case connection is closed before all the data is sent.
    bool sendFrameAscii(const std::string& frame, char delimiter);

    // Close down the connection properly.
    void close();

}; //class ConnectionHandler




#endif