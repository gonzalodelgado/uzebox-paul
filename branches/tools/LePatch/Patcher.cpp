
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <Interpreter.h>
#include "Patcher.h"

Patcher::Patcher(QObject *parent, int timeout)
    : QObject(parent)
{
    procPatch = new QProcess(this);
    connect(procPatch, SIGNAL(started()), this, SLOT(patchStarted()));
    connect(procPatch, SIGNAL(error(QProcess::ProcessError)), this, SLOT(patchError(QProcess::ProcessError)));
    connect(&patchTmr, SIGNAL(timeout()), this, SLOT(patchTimeout()));
    connect(procPatch, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(patchApplied(int,QProcess::ExitStatus)));

    // Timer
    patchTmr.setSingleShot(true);
    patchTmr.setInterval(timeout);
}

void Patcher::applyPatch(QString tool, QString oldFile, QString deltaFile, QString newFile)
{
    QStringList arguments;
    arguments << "-d" << "-s" << oldFile << deltaFile << newFile;
    procPatch->setProcessChannelMode(QProcess::MergedChannels);
    procPatch->setReadChannel(QProcess::StandardOutput);
    procPatch->setWorkingDirectory(QCoreApplication::applicationDirPath());
    emit patchStarted(deltaFile);
    procPatch->start(tool, arguments);
}

void Patcher::patchStarted()
{
    if (patchTmr.isActive())
        patchTmr.stop();
    patchTmr.start();
}

void Patcher::patchError(QProcess::ProcessError error)
{
    Q_ASSERT(interpreter);

    if (patchTmr.isActive())
        patchTmr.stop();
    interpreter->exit(1);
}

void Patcher::patchTimeout()
{
    Q_ASSERT(interpreter);

    if (procPatch && procPatch->isOpen())
        procPatch->close();
}

void Patcher::patchApplied(int exitCode, QProcess::ExitStatus)
{
    Q_ASSERT(interpreter);

    if (patchTmr.isActive()) {
        patchTmr.stop();
        interpreter->exit(exitCode);
    } else {
        interpreter->exit(1);
    }
}

Patcher::~Patcher()
{
    if (procPatch && procPatch->isOpen())
        procPatch->close();
    delete procPatch;
}
