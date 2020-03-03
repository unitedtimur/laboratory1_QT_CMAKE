#include "include/ConsoleFileManager.h"
#include "include/Configuration.h"

#include <QDir>
#include <QTextStream>

#include <thread>

#ifdef Q_OS_WIN32

	#include <windows.h>
	
	static HANDLE hConsole;

#endif

ConsoleFileManager& ConsoleFileManager::instance()
{
	static ConsoleFileManager obj;
	return obj;
}

ConsoleFileManager& ConsoleFileManager::startTerminalThread()
{
	std::thread inputTerminalThread(&ConsoleFileManager::terminal, this);

	inputTerminalThread.detach();

	return *this;
}

ConsoleFileManager& ConsoleFileManager::startCheckFilesThread()
{
	std::thread checkPropertiesThread(&ConsoleFileManager::checkFiles, this);

	checkPropertiesThread.detach();

	return *this;
}

void ConsoleFileManager::fileAdd(const QString& fileName)
{
#ifdef Q_OS_WIN32
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 6);
#elif
	QTextStream(stdout) << "\n\t" << fileName << " was changed!\n\tSize is " << QFileInfo(fileName).size() << " byte" << endl;
#endif
}

void ConsoleFileManager::fileRemove(const QString& index)
{
}

void ConsoleFileManager::fileSize(const QString& index)
{
}

void ConsoleFileManager::terminal()
{
	
}

void ConsoleFileManager::checkFiles()
{
	
}

ConsoleFileManager::ConsoleFileManager(QObject* parent) :
	QObject(parent)
{
	commands = {
		QString("add"),
		QString("remove"),
		QString("size"),
		QString("list"),
		QString("help"),
		QString("clear"),
		QString("remall")
	};
}
