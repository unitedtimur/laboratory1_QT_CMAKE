#include <QCoreApplication>
#include <QTextStream>

#include "include/CheckFile.h"
#include "include/Configuration.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextStream(stdout) << Configuration::Copyright << endl;

    CheckFile checkFile;

    checkFile.startTerminalThread();
    checkFile.startCheckPropertiesThread();

    return a.exec();
}
