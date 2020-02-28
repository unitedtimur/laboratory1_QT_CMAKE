#include <QCoreApplication>
#include <QTextStream>

//#include "include/FileManager.h"
#include "include/ConsoleFileManager.h"
#include "include/Configuration.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextStream(stdout) << Configuration::Copyright << endl;

    ConsoleFileManager::instance().startTerminalThread().startCheckFilesThread();

    return QCoreApplication::exec();
}
