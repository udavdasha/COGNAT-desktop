#include <QApplication>
#include "cognat.h"
#include "database.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    COGNAT cognat;
    cognat.show();
    Database* main_database = cognat.findChild<Database*>(QString(DATABASE_NAME));    
    main_database->cancel(); // This removed nasty window
    return application.exec();
}
