#ifndef PATCHER_H
#define PATCHER_H

#include <QObject>
#include <QProcess>
#include <QTimer>

class Interpreter;

class Patcher : public QObject
{
    Q_OBJECT

public:
    Patcher(QObject *parent, int timeout);
    ~Patcher();

    void setInterpreter(Interpreter *it) { interpreter = it; }

public slots:
    void applyPatch(QString tool, QString oldFile, QString deltaFile, QString newFile);

signals:
    void patchStarted(const QString &patch);
    void patchComplete(bool error);

private slots:
    void patchStarted();
    void patchError(QProcess::ProcessError error);
    void patchApplied(int exitCode, QProcess::ExitStatus);
    void patchTimeout();
private:
    QProcess *procPatch;
    QTimer patchTmr;
    Interpreter *interpreter;
};

#endif // PATCHER_H
