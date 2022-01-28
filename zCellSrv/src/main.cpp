#include <iostream>

#include "network/ctcpserver.h"
#include "thread/cthreadpool.h"
#include <chrono>

int main(int argc, char *argv[])
{
    CTCPServer v;
    if (v.start("127.0.0.1", 5356))
        std::cout << "Server starte" << std::endl;

    zcell_lib::CThreadPool tp;
    tp.start();
    tp.add_job([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        std::cout << "Job1" << std::endl;
    });
    tp.add_job([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        std::cout << "Job2" << std::endl;
    });
    tp.add_job([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        std::cout << "Job3" << std::endl;
    });
    tp.add_job([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        std::cout << "Job4" << std::endl;
    });
    tp.add_job([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        std::cout << "Job5" << std::endl;
    });
    tp.add_job([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        std::cout << "Job6" << std::endl;
    });
    tp.add_job([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        std::cout << "Job7" << std::endl;
    });
    std::this_thread::sleep_for(std::chrono::minutes(2));
    tp.stop();
}
