#include <QApplication>
#include "dialog.h"
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("Home Inc");
    a.setApplicationName("InfoBot");

    QTranslator t;
    t.load("infobot");
    a.installTranslator(&t);

    Dialog w;
    w.show();

    return a.exec();
}
