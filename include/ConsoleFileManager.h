#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QMap>

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
	void fileRemoved(const QString& fileName);
	void enteredSize(const QString& fileName);

private slots:
	void fileAdd(const QString& fileName);
	void fileRemove(const QString& fileName);
	void fileSize(const QString& fileName);
	void printFiles() const;

private:
	explicit ConsoleFileManager(QObject* parent = nullptr);
	~ConsoleFileManager() = default;

	enum Conditions
	{
		Added			= 0x1,
		Removed			= 0x2,
		RemovedWinApi	= 0x3,
		Restored		= 0x4
	};

	QMap<QString, Conditions>				files;
	QMap<QString, qint64>					filesSize;
	QVector <QString>						commands;
};

#endif // FILEMANAGER_H
