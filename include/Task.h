#ifndef TASK__
#define TASK__
#include <boost/thread.hpp>
#include <queue>

class Task{
private:
    int _id;
    boost::mutex * _mutex;
public:
    Task (int id, boost::mutex* mutex) : _id(id), _mutex(mutex) {}

    void run(std::queue<std::string> &lineQueue){
        while(true) {
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize);
            std::string line(buf);

            lineQueue.push(line);
        }
    }
};

#endif //TASK__