#include <QApplication>

#include "mediator.h"
#include <memory>
#include <regex>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    new Mediator();
    return a.exec();
}
