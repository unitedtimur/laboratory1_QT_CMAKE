#include "include/ConsoleFileManager.h"
#include "include/Configuration.h"

#include <QDir>
#include <QTextStream>

#include <thread>

#ifdef Q_OS_WIN32

#include <Windows.h>

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
	SetConsoleTextAttribute(hConsole, 2);
	QTextStream(stdout) << '\t' << fileName << " was added to list" << endl;
	SetConsoleTextAttribute(hConsole, 7);
#elif
	QTextStream(stdout) << '\t' << fileName << " was added to list" << endl;
#endif

	files.insert(fileName, Conditions::Added);
	filesSize.insert(fileName, QFileInfo(fileName).size());
}

void ConsoleFileManager::fileRemove(const QString& fileName)
{
#ifdef Q_OS_WIN32
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 4);
	QTextStream(stdout) << '\t' << fileName << " was removed from list" << endl;
	SetConsoleTextAttribute(hConsole, 7);
#elif
	QTextStream(stdout) << '\t' << fileName << " was removed from list" << endl;
#endif

	files.remove(fileName);
	filesSize.remove(fileName);
}

void ConsoleFileManager::fileSize(const QString& fileName)
{
#ifdef Q_OS_WIN32
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 2);
	QTextStream(stdout) << '\t' << fileName << " size is equal " << QFileInfo(fileName).size() << " byte" << endl;
	SetConsoleTextAttribute(hConsole, 7);
#elif
	QTextStream(stdout) << '\t' << fileName << " size is equal " << QFileInfo(fileName).size() << " byte" << endl;
#endif
}

void ConsoleFileManager::printFiles() const
{
	qint32 iter = 0;

	for (auto it = files.begin(); it != files.end(); ++it)
	{
#ifdef Q_OS_WIN32
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 1);
#endif

		if ((it.value() == Conditions::Added) || (it.value() == Conditions::Restored))
		{
			QTextStream(stdout) << '\t' << ++iter << " ---> " << it.key() << endl;
		}
		else
		{
			QTextStream(stdout) << '\t' << ++iter << " ---> " << it.key() << " REMOVED WINAPI" << endl;
		}

#ifdef Q_OS_WIN32
		SetConsoleTextAttribute(hConsole, 7);
#endif
	}
}

void ConsoleFileManager::terminal()
{
	QTextStream cin(stdin), cout(stdout);

	forever
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		QString command;

		bool isCommand = false;

		while (!isCommand)
		{
			cout << flush << Configuration::MessageInputTheCommand << flush;
			command = cin.readLine().trimmed().toLower();
			std::find(commands.begin(), commands.end(), command) != commands.end() ? isCommand = true : isCommand = false;

			if (!isCommand && command.isEmpty())
			{
				command = commands[4];
				isCommand = true;
			}
		}

		// command == 'add'
		if (command == commands[0])
		{
			QString pathToFile;
			QFileInfoList list;

			bool isAdd = false;

			while (!isAdd)
			{
				cout << '\t' << Configuration::MessageAdd << flush;
				pathToFile = cin.readLine().trimmed();

				if (std::find(pathToFile.begin(), pathToFile.end(), '"') != pathToFile.end())
					pathToFile = pathToFile.trimmed().mid(1, pathToFile.size() - 2).toUtf8();

				if (QFileInfo(pathToFile).exists())
				{
					if (QFileInfo(pathToFile).isDir())
					{
						QDir dir(pathToFile);
						list = dir.entryInfoList(QDir::Files);

						if (list.isEmpty())
						{
							break;
						}

						for (const auto& it : list)
						{
							emit fileAdded(it.filePath());
						}

						isAdd = true;
					}
					else
					{
						if (QFileInfo(pathToFile).isFile())
						{
							emit fileAdded(pathToFile);
							isAdd = true;
						}
					}
				}
			}

			continue;
		}

		// command == 'remove'
		if (command == commands[1])
		{
			if (!files.isEmpty())
			{
				cout << '\t' << Configuration::MessageRemove << endl;
				printFiles();

				qint32 number = 0;

				bool isRemove = false;

				while (!isRemove)
				{
					cout << Configuration::MessageInputNumber << flush;
					number = cin.readLine().toInt();

					if (number > 0 && number <= files.size())
					{
						qint32 index = 0;

						for (auto it = files.begin(); it != files.end(); ++it)
						{
							if (index++ == number - 1)
							{
								emit fileRemoved(it.key());
								isRemove = true;
								break;
							}
						}
					}
				}
			}
			else
			{
				cout << '\t' << Configuration::MessageEmptyList << endl;
			}

			continue;
		}

		//command == 'size'
		if (command == commands[2])
		{
			if (!files.isEmpty())
			{
				cout << '\t' << Configuration::MessageSize << endl;
				printFiles();

				qint32 number = 0;

				bool isSize = false;

				while (!isSize)
				{
					cout << Configuration::MessageInputNumber << flush;
					number = cin.readLine().toInt();

					if (number > 0 && number <= files.size())
					{
						qint32 index = 0;

						for (auto it = files.begin(); it != files.end(); ++it)
						{
							if (index++ == number - 1)
							{
								emit enteredSize(it.key());
								isSize = true;
								break;
							}
						}
					}
				}
			}
			else
			{
				cout << '\t' << Configuration::MessageEmptyList << endl;
			}

			continue;
		}

		// command == 'list'
		if (command == commands[3])
		{
			if (!files.isEmpty())
			{
				cout << '\t' << Configuration::MessageList << endl;
				printFiles();
			}
			else
			{
				cout << '\t' << Configuration::MessageEmptyList << endl;
			}

			continue;
		}

		// command == 'help'
		if (command == commands[4])
		{
			QStringList help = Configuration::MessageHelp.split('\n');

			foreach(const auto & it, help)
			{
				cout << '\t' << it << endl;
			}

			continue;
		}

		// command == 'clear'
		if (command == commands[5])
		{
			#ifdef Q_OS_WIN32
				system("cls");
			#endif

			#ifdef Q_OS_LINUX
				system("clear");
			#endif

			continue;
		}

		// command == 'remAll'
		if (command == commands[6])
		{
			if (!files.isEmpty())
			{
				files.clear();
				cout << '\t' << Configuration::MessageRemAll << endl;
			}
			else
			{
				cout << '\t' << Configuration::MessageEmptyList << endl;
			}
		}
	}
}

void ConsoleFileManager::checkFiles()
{
	forever
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		QTextStream cout(stdout);

		if (!files.isEmpty())
		{
			bool isCheck = false;;

			for (auto it = files.begin(); it != files.end(); ++it)
			{
				if (!QFileInfo(it.key()).exists() && it.value() != Conditions::Removed)
				{
					it.value() = Conditions::RemovedWinApi;
				}

				if (QFileInfo(it.key()).exists() && it.value() == Conditions::Removed)
				{
					isCheck = true;
					it.value() = Conditions::Restored;

					#ifdef Q_OS_WIN32
					hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(hConsole, 2);
					cout << endl << "\tFile " << it.key() << " was restored!" << endl;
					SetConsoleTextAttribute(hConsole, 7);
					#elif
					cout << endl << "\tFile " << it.key() << " was restored!" << endl;
					#endif
				}

				if (it.value() == Conditions::RemovedWinApi)
				{
					isCheck = true;

					#ifdef Q_OS_WIN32
					hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(hConsole, 4);
					cout << endl << "\tFile " << it.key() << " was removed with WinApi!" << endl;
					SetConsoleTextAttribute(hConsole, 7);
					#elif
					cout << endl << "\tFile " << it.key() << " was removed with WinApi!" << endl;
					#endif

					it.value() = Conditions::Removed;
				}
			}

			for (auto it = filesSize.begin(); it != filesSize.end(); ++it)
			{
				if (it.value() != QFileInfo(it.key()).size() && files[it.key()] != Conditions::Removed)
				{
					isCheck = true;

					#ifdef Q_OS_WIN32
					hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
					SetConsoleTextAttribute(hConsole, 6);
					cout << endl << "\tFile " << it.key() << " was changed!" << endl;
					SetConsoleTextAttribute(hConsole, 7);
					#elif
					cout << endl << "\tFile " << it.key() << " was changed!" << endl;
					#endif

					emit fileSize(it.key());
					it.value() = QFileInfo(it.key()).size();
				}
			}

			if (isCheck)
			{
				cout << Configuration::MessageInputTheCommand << flush;
			}
		}
	}
}

ConsoleFileManager::ConsoleFileManager(QObject* parent) :
	QObject(parent),
	commands {
		QString("add"),
		QString("remove"),
		QString("size"),
		QString("list"),
		QString("help"),
		QString("clear"),
		QString("remall")
}
{
	connect(this, &ConsoleFileManager::fileAdded, this, &ConsoleFileManager::fileAdd);
	connect(this, &ConsoleFileManager::fileRemoved, this, &ConsoleFileManager::fileRemove);
	connect(this, &ConsoleFileManager::enteredSize, this, &ConsoleFileManager::fileSize);
}