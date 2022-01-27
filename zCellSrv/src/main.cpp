#include <iostream>

#include "network/ctcpserver.h"
#include "thread/cthreadpool.h"
#include <chrono>

int main(int argc, char *argv[])
{
    CTCPServer v;
    if (v.start("127.0.0.1", 5356))
        std::cout << "Server starte" << std::endl;

    std::cout << "Creating thread pool" << std::endl;
    zcell_lib::CThreadPool tp;
    std::cout << "Optimal thread count\t" << tp.optimal_thread_count() << std::endl;
    std::cout << "Number of threads in pool\t" << tp.num_threads() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Resizing pool" << std::endl;
    tp.set_num_threads(5);
    std::cout << "Number of threads in pool\t" << tp.num_threads() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Starting threads..." << std::endl;
    tp.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Resizing pool" << std::endl;
    tp.set_num_threads(8);
    std::cout << "Number of threads in pool\t" << tp.num_threads() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Resizing pool" << std::endl;
    tp.set_num_threads(3);
    std::cout << "Number of threads in pool\t" << tp.num_threads() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Stopping threads..." << std::endl;
    tp.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    std::cout << "=====End=====" << std::endl;
}
