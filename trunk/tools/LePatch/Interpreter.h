#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QThread>
#include <QWaitCondition>
#include <QReadWriteLock>
#include <QQueue>
#include <QStack>
#include <QDir>
#include <Patcher.h>

class Patcher;

class Interpreter : public QThread
{
    Q_OBJECT

public:
    static const QString UPDATES_DIR;
    static const QString BACKUP_DIR;

    Interpreter(QObject *parent = 0, int timeout = 5000);
    ~Interpreter();

    void setVersion(const QString &v);
    void setUpdateScript(const QString &script);
    bool revertUpdate();

public slots:
    void applyUpdate();
    void cancelUpdate();
    void setTimeout(int tms) { timeout = tms; }

signals:
    void updateScriptParsed(bool error);
    void commandCountInitialised(int count);
    void commandCountChanged(int count);
    void fileMoveStarted(QString from, QString to);
    void fileMoveFinished(bool error);
    void fileRemovalStarted(QString fileName);
    void fileRemovalFinished(bool error);
    void fileCopyStarted(QString from, QString to);
    void fileCopyFinished(bool error);
    void filePatchStarted(QString fileName);
    void filePatchFinished(bool error);
    void dirMakeStarted(QString dirName);
    void dirMakeFinished(bool error);
    void dirRemoveStarted(QString dirName);
    void dirRemoveFinished(bool error);
    void dirCopyStarted(QString from, QString to);
    void dirCopyFinished(bool error);
    void dirRestoreStarted(QString dirName);
    void dirRestoredFinished(bool error);
    void updateStarted();
    void updateFinished(bool error);
    void updateReversionStarted();
    void updateReversionFinished(bool error);
    void applyPatch(QString tool, QString oldFile, QString deltaFile, QString newFile);
    void fatalError(QString errMsg);

protected:
    void run();

private:
    bool isPatchCancelled();
    int convertVersion(const QString &v);
    bool validateUpdateScript(const QString &script);
    bool validateCmd(const QStringList &cmd);
    bool isChildPath(const QString &path, const QDir &dir = QDir::current());
    bool moveFile(const QString &from, const QString &to);
    bool removeFile(const QString &fileName, const QString &backupName = "");
    bool copyFile(const QString &from, const QString &to);
    bool makeDir(const QString &dirPath);
    bool removeDir(const QString &dirName, const QString &backupDirName = "");
    bool copyDir(const QString &from, const QString &to);
    bool restoreDir(const QString &origDirName, const QString &currDirName);
    bool patchFile(const QString &tool, const QString &oldFile, const QString &deltaFile, const QString &newFile);

    QWaitCondition condition;
    QReadWriteLock rwlock;
    QQueue<QStringList> cmds;
    QStack<QStringList> revertCmds;
    int timeout;
    Patcher *patcher;
    int version;
    QString updateScript;
    // Shared
    bool cancel;
    bool patchResult;
};

#endif // INTERPRETER_H
