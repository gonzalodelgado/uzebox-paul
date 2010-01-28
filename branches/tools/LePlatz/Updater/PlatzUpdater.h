#ifndef PLATZUPDATER_H
#define PLATZUPDATER_H

#include <QObject>
#include <QHttp>
#include <QQueue>
#include <QDir>
#include <QUrl>
#include <QXmlStreamReader>

class PlatzUpdater : public QObject
{
    Q_OBJECT
public:
    struct UpdateFile {
        QFile file;
        QUrl url;

        UpdateFile()
            : file(""), url("")
        {}
    };

    PlatzUpdater(QWidget *parent = 0);
    ~PlatzUpdater();

    bool downloadUpdates(const QString &versionUrl);

private slots:
    bool beginDownload();
    void httpRequestFinished(int requestId, bool error);
    void readResponseHeader(const QHttpResponseHeader &responseHeader);
    void updateDataReadProgress(int bytesRead, int totalBytes);

signals:
    void progressChanged(const QString &fileName, int bytesRead, int totalBytes, int fileIndex, int totalFiles);
    void downloadComplete(bool error, const QString &errMsg);
private:
    enum UpdateState {
        Idle,
        VersionCheck,
        Downloading
    };

    bool isChildPath(const QString &path, const QDir &dir = QDir::current());
    bool downloadUpdateFile();
    bool parseUpdateFile(QIODevice *device);
    void parseFiles(QXmlStreamReader &reader);

    QWidget *baseWin;
    QHttp *http;
    UpdateFile *update;
    QQueue<UpdateFile*> updatesQ;
    QList<UpdateFile*> updatedList;
    int httpGetId;
    bool httpRequestAborted;
    UpdateState state;
};

#endif // PLATZUPDATER_H
