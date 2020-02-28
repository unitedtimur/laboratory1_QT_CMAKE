#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QFileSystemWatcher>

class FileManager final : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager() = default;

    void startTerminalThread();
    void startCheckPropertiesThread();

signals:
    void fileAdded(const QString& fileName);
    void fileRemoved(const qint32& index);
    void enteredSize(const qint32& index);

private slots:
    void Q_NORETURN terminal();
    void Q_NORETURN checkProperties();

private:
    QVector<QString>    commands;
    QVector<QString>    fileNames;
    QFileSystemWatcher  fileSystemWatcher;
};

#endif // FILEMANAGER_H
