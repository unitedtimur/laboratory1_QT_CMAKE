#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QFileSystemWatcher>

class ConsoleFileManager final : public QObject
{
	Q_OBJECT

public:
	static ConsoleFileManager& instance();

	ConsoleFileManager& startTerminalThread();
	ConsoleFileManager& startCheckFilesThread();

signals:
	void fileAdded(const QString& fileName);
	void fileRemoved(const qint32& index);
	void enteredSize(const qint32& index);

private slots:
	void fileAdd(const QString& fileName);
	void fileRemove(const qint32& index);
	void fileSize(const qint32& index);
	void Q_NORETURN terminal();
	void Q_NORETURN checkFiles();

private:
	explicit ConsoleFileManager(QObject* parent = nullptr);
	explicit ConsoleFileManager(const ConsoleFileManager&) = delete;
	ConsoleFileManager& operator=(const ConsoleFileManager&) = delete;
	~ConsoleFileManager() = default;

	QVector<QString>	fileNames;
	QVector<QString>	commands;
	QFileSystemWatcher	fileSystemWatcher;
};

#endif // FILEMANAGER_H
