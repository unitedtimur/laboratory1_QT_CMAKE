#include <QCoreApplication>
#include <QDebug>

#include "include/CheckFile.h"
#include "include/Configuration.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextStream(stdout) << Configuration::Copyright << endl;

    CheckFile checkFile;

    checkFile.startTerminalThread();
    checkFile.startCheckPropertiesThread();

    //QVector temp = {1, 2, 3};

    return a.exec();
}
