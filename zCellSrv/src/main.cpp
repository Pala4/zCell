#include <QCoreApplication>

#include "samp.h"
#include "zcellsrvlib.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ZCellSrvLib v;
    v.print();

    return a.exec();
}
