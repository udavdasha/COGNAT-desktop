#include <QApplication>
#include "cognat.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    COGNAT cognat;
    cognat.show();
    return application.exec();
}
