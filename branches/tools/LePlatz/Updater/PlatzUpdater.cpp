
#include <QtNetwork>
#include <PlatzEnums.h>
#include "PlatzUpdater.h"

// This file adapted from original Qt Http Client example
PlatzUpdater::PlatzUpdater(QWidget *parent)
    : baseWin(parent), update(0), state(Idle)
{
    http = new QHttp(this);
    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));
    connect(http, SIGNAL(dataReadProgress(int, int)), this, SLOT(updateDataReadProgress(int, int)));
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)), this, SLOT(readResponseHeader(const QHttpResponseHeader&)));
}

bool PlatzUpdater::downloadUpdates(const QString &versionUrl)
{
    update = new UpdateFile();
    update->file.setFileName("LePlatz-Updates.xml");
    update->url.setUrl(versionUrl);
    state = VersionCheck;

    bool retval = beginDownload();

    if (!retval)
        state = Idle;
    return retval;
}

void PlatzUpdater::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
    switch (responseHeader.statusCode()) {
    case 200:                   // Ok
    case 301:                   // Moved Permanently
    case 302:                   // Found
    case 303:                   // See Other
    case 307:                   // Temporary Redirect
        // these are not error conditions
        break;
    default:
        httpRequestAborted = true;

        if (http)
            http->abort();
        break;
    }
}

void PlatzUpdater::updateDataReadProgress(int bytesRead, int totalBytes)
{
    if (httpRequestAborted || state != Downloading || !update)
        return;
    emit progressChanged(update->file.fileName(), bytesRead, totalBytes, updatedList.count(), updatesQ.count() + updatedList.count() + 1);
}

// Ensure a rogue script can't mess with files outside of the LePlatz directory tree.
bool PlatzUpdater::isChildPath(const QString &path, const QDir &dir)
{
    return !dir.relativeFilePath(path).startsWith("..");
}

bool PlatzUpdater::beginDownload()
{
    if (!update || state == Idle)
        return false;

    QDir dir(QDir::currentPath() + "/updates");

    if (!dir.exists()) {
        dir.setPath(QDir::currentPath());

        if (!dir.mkpath("updates"))
            return false;
    }
    QFileInfo fileInfo(update->url.toString());

    if (fileInfo.fileName().isEmpty())
        return false;
    QString fileName = dir.absolutePath() + "/" + update->file.fileName();
    QDir dirSave(fileName.left(fileName.lastIndexOf("/")));

    update->file.setFileName(fileName);

    if (!dirSave.exists()) {
        if (!dirSave.mkpath(dirSave.absolutePath()))
            return false;
    }

    if (!update->file.open(QIODevice::WriteOnly))
        return false;
    QHttp::ConnectionMode mode = update->url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
    http->setHost(update->url.host(), mode, update->url.port() == -1 ? 0 : update->url.port());
    httpRequestAborted = false;
    QByteArray path = QUrl::toPercentEncoding(update->url.path(), "!$&'()*+,;=:@/");

    if (path.isEmpty())
        path = "/";
    httpGetId = http->get(path, &update->file);
    return true;
}

bool PlatzUpdater::downloadUpdateFile()
{
    if (!update)
        return false;
    return beginDownload();
}

bool PlatzUpdater::parseUpdateFile(QIODevice *device)
{
    if (!device || !device->open(QFile::ReadOnly | QFile::Text))
        return false;
    QString s;
    QXmlStreamReader reader(device);

    while(!reader.atEnd()) {
        reader.readNext();

        if (reader.isStartElement()) {
            s = reader.name().toString();

            if (s == "LePlatzUpdates") {
                ; // Do nothing
            } else if (s == "Update") {
                if (reader.attributes().count() == 0 || Platz::LEPLATZ_VERSIONS.contains(reader.attributes().value("version").toString()))
                    reader.skipCurrentElement();
            } else if (s == "Files") {
                parseFiles(reader);
            } else {
                reader.skipCurrentElement();
            }
        }
    }
    device->close();
    return !reader.error();
}

void PlatzUpdater::parseFiles(QXmlStreamReader &reader)
{
    Q_ASSERT(reader.isStartElement() && reader.name() == "Files");

    QString s;

    while(!reader.atEnd()) {
        reader.readNext();

        if (reader.isEndElement() && reader.name() == "Files") {
            break;
        } else if (reader.isStartElement()) {
            s = reader.name().toString();

            if (s == "File") {
                UpdateFile *uf = new UpdateFile();
                updatesQ.enqueue(uf);
            } else if (s == "Name") {
                if (updatesQ.isEmpty())
                    break;
                updatesQ.last()->file.setFileName(reader.readElementText());

                if (!isChildPath(updatesQ.last()->file.fileName()))
                    reader.raiseError();
            } else if (s == "Url") {
                if (updatesQ.isEmpty())
                    break;
                updatesQ.last()->url.setUrl(reader.readElementText());
            } else {
                reader.skipCurrentElement();
            }
        }
    }
}

void PlatzUpdater::httpRequestFinished(int requestId, bool error)
{
    Q_ASSERT(state != Idle && update);

    if (requestId != httpGetId)
        return;
    if (state == VersionCheck) {
        update->file.close();

        if (error || httpRequestAborted) {
            update->file.remove();
            emit downloadComplete(error, http->errorString());
            deleteLater();
            return;
        }

        if (!parseUpdateFile(&update->file) || updatesQ.isEmpty()) {
            emit downloadComplete(true, "Invalid update file format.");
            deleteLater();
            return;
        } else {
            state = Downloading;
            update = updatesQ.dequeue();

            if (!downloadUpdateFile()) {
                emit downloadComplete(true, "Incomplete update file.");
                deleteLater();
            }
        }
    } else if (state == Downloading) {
        if (error || httpRequestAborted) {
            update->file.remove();

            foreach (UpdateFile *uf, updatedList)
                uf->file.remove();
            emit downloadComplete(error, http->errorString());
            deleteLater();
            return;
        }
        update->file.close();
        updatedList.append(update);

        if (!updatesQ.isEmpty()) {
            update = updatesQ.dequeue();

            if (!downloadUpdateFile()) {
                emit downloadComplete(true, "Incomplete update file.");
                deleteLater();
            }
        } else {
            emit downloadComplete(error, http->errorString());
            deleteLater();
        }
    }
}

PlatzUpdater::~PlatzUpdater()
{
    if (state != Idle && http && http->state() != QHttp::Unconnected)
        http->abort();
    delete http;
    http = 0;

    UpdateFile *it = 0;

    while (!updatesQ.isEmpty()) {
        it = updatesQ.dequeue();

        if (it != update)
            delete it;
    }
    while (!updatedList.isEmpty()) {
        it = updatedList.takeFirst();

        if (it != update)
            delete it;
    }
    delete update;
    update = 0;
}
