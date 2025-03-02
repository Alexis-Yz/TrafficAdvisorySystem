#include "trafficadvisorysystem.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TrafficAdvisorySystem w;
    w.show();
    return a.exec();
}
