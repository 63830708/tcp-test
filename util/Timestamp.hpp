#ifndef _TIME_STAMP_H
#define _TIME_STAMP_H

#include <chrono>

using namespace std::chrono;

class Timestamp
{
public:
    Timestamp() { start();}
    ~Timestamp(){}

    void start()
    {
        begin = high_resolution_clock::now();
    }

    long long elapsed()
    {
        return duration_cast<microseconds>(high_resolution_clock::now() - begin).count();
    }

    double elaspedS()
    {
        return elapsed()*0.000001;
    }
    double elapsedMs()
    {
        return elapsed()*0.001;
    }
private:
    time_point<high_resolution_clock> begin;


};


#endif
