#include "network/ctcpsocket.h"

int main(int argc, char *argv[])
{
    CTcpSocket soc;
    soc.connetTo("127.0.0.1", 5356);

    return -1;
}
