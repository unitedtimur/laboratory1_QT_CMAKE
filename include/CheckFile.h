#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <QObject>
#include <QFileSystemWatcher>

class CheckFile final : public QObject
{
    Q_OBJECT
public:
    explicit CheckFile(QObject *parent = nullptr);
    ~CheckFile() = default;

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

#endif // CHECKFILE_H
