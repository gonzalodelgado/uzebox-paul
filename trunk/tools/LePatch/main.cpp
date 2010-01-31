
#include <QtGui/QApplication>
#include <QStringList>
#include "LePatchDlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = QApplication::arguments();
    LePatchDlg w;
    int it = 1;
    QString version("1.0"), updateScript("");

    while (it < args.count()) {
        if (args.at(it) == "-v") {
            if (++it < args.count()) {
                version = args.at(it);
                break;
            }
        }
        ++it;
    }

    if (updateScript.isEmpty())
        updateScript = "LePlatz-Updates.xml";
    w.show();
    w.setUpdateScript(updateScript);
    w.setVersion(version);
    return a.exec();
}
