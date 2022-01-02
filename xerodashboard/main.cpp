#include "xerodashboard.h"
#include <QtWidgets/QApplication>
#include <QtCore/QStandardPaths>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("ErrorCodeXero");
    QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
    QCoreApplication::setApplicationName("XeroDashBoard");
    QCoreApplication::setApplicationVersion("1.0.0");

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    XeroDashBoard w;
    w.show();
    return a.exec();
}
