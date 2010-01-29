#ifndef LEPATCHDLG_H
#define LEPATCHDLG_H

#include <QDialog>
#include <Interpreter.h>

namespace Ui {
    class LePatchDlg;
}

class LePatchDlg : public QDialog {
    Q_OBJECT
public:
    LePatchDlg(QWidget *parent = 0);
    ~LePatchDlg();
    
    void setVersion(const QString &version);
    void setUpdateScript(const QString &script);

public slots:
    void scriptParseComplete(bool error);
    void setCommandCount(int count);
    void setCommandsRemaining(int count);
    void fileMoveStarted(QString from, QString to);
    void fileRemovalStarted(QString fileName);
    void fileCopyStarted(QString from, QString to);
    void filePatchStarted(QString fileName);
    void dirMakeStarted(QString dirName);
    void dirRemoveStarted(QString dirName);
    void dirCopyStarted(QString from, QString to);
    void dirRestoreStarted(QString dirName);
    void updateStarted();
    void updateFinished(bool error);
    void updateReversionStarted();
    void updateReversionFinished(bool error);
    void commandComplete(bool error);
    void fatalError(QString msg);

private slots:
    void toggleDetails();
    void applyUpdate();
    void exitProgram();

private:
    void printLn(const QString &s);
    void print(const QString &s);

    Ui::LePatchDlg *ui;
    Interpreter *interpreter;
    bool updating;
};

#endif // LEPATCHDLG_H
