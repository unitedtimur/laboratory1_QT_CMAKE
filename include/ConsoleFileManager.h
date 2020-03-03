#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QMap>
#include <QFileSystemWatcher>

class ConsoleFileManager final : public QObject
{
	Q_OBJECT

public:
	explicit ConsoleFileManager(const ConsoleFileManager&) = delete;
	ConsoleFileManager& operator=(const ConsoleFileManager&) = delete;
	static ConsoleFileManager& instance();

	ConsoleFileManager& startTerminalThread();
	ConsoleFileManager& startCheckFilesThread();

	void Q_NORETURN terminal();
	void Q_NORETURN checkFiles();

signals:
	void fileAdded(const QString& fileName);
	void fileRemoved(const QString& index);
	void enteredSize(const QString& index);

private slots:
	void fileAdd(const QString& fileName);
	void fileRemove(const QString& index);
	void fileSize(const QString& index);

private:
	explicit ConsoleFileManager(QObject* parent = nullptr);
	~ConsoleFileManager() = default;

	QMap<QString, bool> files;
	QVector<QString>	commands;
};

#endif // FILEMANAGER_H
