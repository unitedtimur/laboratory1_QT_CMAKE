#include "include/ConsoleFileManager.h"
#include "include/Configuration.h"

#include <QDir>
#include <QTextStream>

#include <thread>

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
	if (QFileInfo(fileName).exists())
	{
		QTextStream(stdout) << '\t' << fileName << " was added to list" << endl;
		fileNames.push_back(fileName);
	}
}

void ConsoleFileManager::fileRemove(const qint32& index)
{
	QTextStream(stdout) << '\t' << fileNames[index] << " was removed from list" << endl;
	fileNames.remove(index);
}

void ConsoleFileManager::fileSize(const qint32& index)
{
	QTextStream(stdout) << '\t' << fileNames[index] << " size is equal " << QFileInfo(fileNames[index]).size() << " byte" << endl;
}

void ConsoleFileManager::terminal()
{
	QTextStream cin(stdin), cout(stdout);

	const auto printListFiles = [&]() -> void
	{
		qint32 iter = 0;

		foreach(const auto & fileName, fileNames)
		{
			cout << '\t' << ++iter << " ---> " << fileName << endl;
		}
	};

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
			bool isDir = false;
			bool isAdd = false;

			while (!isAdd)
			{
				cout << '\t' << Configuration::MessageAdd << flush;
				pathToFile = cin.readLine().trimmed();

				if (std::find(pathToFile.begin(), pathToFile.end(), '"') != pathToFile.end())
					pathToFile = pathToFile.trimmed().mid(1, pathToFile.size() - 2).toUtf8();

				cout.setCodec("IBM866");
				cout << pathToFile.toUtf8() << endl;

				if (QFileInfo(pathToFile).exists())
				{
					if (QFileInfo(pathToFile).isFile() && std::find(fileNames.begin(), fileNames.end(), pathToFile) == fileNames.end())
					{
						isAdd = true;
					}
					else
					{
						isAdd = false;

						if (QFileInfo(pathToFile).isDir() && std::find(fileNames.begin(), fileNames.end(), pathToFile) == fileNames.end())
						{
							isDir = true;
							isAdd = true;

							QDir dir(pathToFile);
							list = dir.entryInfoList(QDir::Files);

							if (list.isEmpty())
							{
								isDir = false;
								isAdd = false;
							}

							for (const auto& it : list)
							{
								if (std::find(fileNames.begin(), fileNames.end(), it.filePath()) != fileNames.end())
								{
									isDir = false;
									isAdd = false;
									break;
								}
							}
						}
					}
				}
				else
				{
					isAdd = false;
				}
			}

			if (isDir)
			{
				for (const auto& it : list)
				{
					emit fileAdded(it.filePath());
				}
			}
			else
			{
				emit fileAdded(pathToFile);
			}

			continue;
		}

		// command == 'remove'
		if (command == commands[1])
		{
			if (!fileNames.isEmpty())
			{
				cout << flush << '\t' << Configuration::MessageRemove << endl;
				printListFiles();

				qint32 number = 0;

				bool isRemove = false;

				while (!isRemove)
				{
					cout << Configuration::MessageInputNumber << flush;
					number = cin.readLine().toInt();

					if (number > 0 && number <= fileNames.size())
					{
						emit fileRemoved(--number);
						isRemove = true;
					}
					else
					{
						cout << Configuration::MessageFixProblem << endl;
					}
				}
			}
			else
			{
				cout << flush << '\t' << Configuration::MessageEmptyList << endl;
			}

			continue;
		}

		// command == 'size'
		if (command == commands[2])
		{
			if (!fileNames.isEmpty())
			{
				cout << flush << '\t' << Configuration::MessageSize << endl;
				printListFiles();

				qint32 number = 0;

				bool isSize = false;

				while (!isSize)
				{
					cout << Configuration::MessageInputNumber << flush;
					number = cin.readLine().toInt();

					if (number > 0 && number <= fileNames.size())
					{
						emit enteredSize(--number);
						isSize = true;
					}
				}
			}
			else
			{
				cout << flush << '\t' << Configuration::MessageEmptyList << endl;
			}

			continue;
		}

		// command == 'list'
		if (command == commands[3])
		{
			if (!fileNames.isEmpty())
			{
				cout << flush << '\t' << Configuration::MessageList << endl;
				printListFiles();
			}
			else
			{
				cout << flush << '\t' << Configuration::MessageEmptyList << endl;
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
			if (!fileNames.isEmpty())
			{
				for (qint32 i = fileNames.size(); i > 0; --i)
				{
					emit fileRemoved(i - 1);
				}

				QTextStream(stdout) << "\tAll files have been removed!" << endl;
			}
			else
			{
				cout << flush << '\t' << Configuration::MessageEmptyList << endl;
			}

			continue;
		}
	}
}

void ConsoleFileManager::checkFiles()
{
	forever
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		QTextStream cout(stdout);

		if (!fileNames.isEmpty())
		{
			qint32 iter = 0;

			bool isDelete = false;

			for (qint32 i = fileNames.size() - 1; i >= 0; --i)
			{
				if (!QFileInfo(fileNames[i]).exists())
				{
					isDelete = true;
					cout << endl;
					emit fileRemoved(i);
				}
			}

			if (isDelete)
			{
				cout << flush << Configuration::MessageInputTheCommand;
			}
		}

		cout << flush;
	}
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

	connect(&fileSystemWatcher, &QFileSystemWatcher::fileChanged, [&](const QString& fileName)
		{
			if (QFileInfo(fileName).exists())
			{
				QTextStream(stdout) << "\n\t" << fileName << " was changed!\n\tSize is " << QFileInfo(fileName).size() << " byte" << endl;
				QTextStream(stdout) << Configuration::MessageInputTheCommand;
			}
		});

	connect(this, &ConsoleFileManager::fileAdded, this, &ConsoleFileManager::fileAdd);

	connect(this, &ConsoleFileManager::fileRemoved, this, &ConsoleFileManager::fileRemove);

	connect(this, &ConsoleFileManager::enteredSize, this, &ConsoleFileManager::fileSize);
}
