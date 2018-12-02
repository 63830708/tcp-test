#ifndef MY_UTIL_H
#define MY_UTIL_H


#include <WinSock2.h>
#include <QDebug>
#include <string>
#include <iostream>
#include <iomanip>

//#pragma comment(lib, "ws2_32.lib")
#pragma execution_character_set("utf-8")

#define LOG qDebug()


using std::string;
using std::cin;


struct Data
{
    int age;
    char name[128];
};


const char *SERVER_IP = "192.168.1.198";
const int SERVER_PORT = 8899;

#endif
