#include <QCoreApplication>

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Hello world" << std::endl;
    int i = 0;
    i++;

    return a.exec();
}
