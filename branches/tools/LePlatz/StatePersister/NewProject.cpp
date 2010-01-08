
/*
 *  LePlatz - A level editor for the Platz toolset (Uzebox - supports VIDEO_MODE 3)
 *  Copyright (C) 2009 Paul McPhee
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <PlatzEnums.h>
#include "NewProject.h"
#include "ui_NewProject.h"

using namespace Platz;

NewProject::NewProject(Settings *settings, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::NewProject), settings(settings)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // Initial settings
    ui->lblLogoImg->setPixmap(QPixmap(":/misc/LePlatzLogo.png"));
    ui->lblContextualHelp->setText("");

    ui->cboVideoMode->addItem("Mode 2 (192x208)", QVariant(QSize(192, 208)));
    ui->cboVideoMode->addItem("Mode 3 (256x224)", QVariant(QSize(256, 224)));
    connect(ui->cboVideoMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setOverlayRange(int)));
    ui->cboVideoMode->setCurrentIndex(1);

    ui->cboImageFormat->addItems(Platz::SUPPORTED_IMAGE_FORMATS_EXTS);
    ui->cboImageFormat->setCurrentIndex(ui->cboImageFormat->count()-1);
    displayDefaultHelp();

    // Connections
    connect(ui->pbArtPath, SIGNAL(pressed()), this, SLOT(artFolderFileDialog()));
    connect(ui->pbSrcPath, SIGNAL(pressed()), this, SLOT(srcFolderFileDialog()));
    connect(ui->pbProjectPath, SIGNAL(pressed()), this, SLOT(projectFolderFileDialog()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(createNewProject()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui->leProjectName, SIGNAL(receivedFocus()), this, SLOT(displayProjectNameHelp()));
    connect(ui->leProjectName, SIGNAL(focusLost()), this, SLOT(displayDefaultHelp()));
    connect(ui->leProjectFolder, SIGNAL(receivedFocus()), this, SLOT(displayProjectFolderHelp()));
    connect(ui->leProjectFolder, SIGNAL(focusLost()), this, SLOT(displayDefaultHelp()));
    connect(ui->leArtFolder, SIGNAL(receivedFocus()), this, SLOT(displayArtFolderHelp()));
    connect(ui->leArtFolder, SIGNAL(focusLost()), this, SLOT(displayDefaultHelp()));
    connect(ui->leSrcFolder, SIGNAL(receivedFocus()), this, SLOT(displaySrcFolderHelp()));
    connect(ui->leSrcFolder, SIGNAL(focusLost()), this, SLOT(displayDefaultHelp()));
    connect(ui->cboVideoMode, SIGNAL(receivedFocus()), this, SLOT(displayVideoModeHelp()));
    connect(ui->cboVideoMode, SIGNAL(focusLost()), this, SLOT(displayDefaultHelp()));
    connect(ui->spbOverlayLines, SIGNAL(valueChanged(int)), this, SLOT(setVideoModeRes(int)));
    connect(ui->spbOverlayLines, SIGNAL(receivedFocus()), this, SLOT(displayOverlayLinesHelp()));
    connect(ui->spbOverlayLines, SIGNAL(focusLost()), this, SLOT(displayDefaultHelp()));
    ui->leProjectName->setFocus();
}

void NewProject::setVideoModeRes(int overlayLines)
{
    Q_ASSERT(ui->cboVideoMode->count() > 1);
    int hgt0 = ui->cboVideoMode->itemData(0, Qt::UserRole).toSize().height();
    int hgt1 = ui->cboVideoMode->itemData(1, Qt::UserRole).toSize().height();
    ui->cboVideoMode->setItemText(0, QString("Mode 2 (192x%1)").arg(qMax(0, hgt0-8*overlayLines)));
    ui->cboVideoMode->setItemText(1, QString("Mode 3 (256x%1)").arg(hgt1-8*overlayLines));
}

void NewProject::setOverlayRange(int index)
{
    if (ui->cboVideoMode->count() > index) {
        if (index == 0) {
            ui->spbOverlayLines->setValue(qMin(ui->spbOverlayLines->value(), settings->VMODE2_SCREEN_TILES_V));
            ui->spbOverlayLines->setMaximum(settings->VMODE2_SCREEN_TILES_V);
        } else {
            ui->spbOverlayLines->setValue(qMin(ui->spbOverlayLines->value(), settings->VMODE3_SCREEN_TILES_V));
            ui->spbOverlayLines->setMaximum(settings->VMODE3_SCREEN_TILES_V);
        }
    }
}

void NewProject::displayDefaultHelp()
{
    ui->lblContextualHelp->setText("All settings may be changed and refined after the project has been created. "
                                   "LePlatz assumes a default project path to simplify project creation.");
}

void NewProject::displayProjectNameHelp()
{
    ui->lblContextualHelp->setText("The project will be saved as an xml file under this name. By default, "
                                   "the hex file will be considered to exist under this same name.");
}

void NewProject::displayProjectFolderHelp()
{
    ui->lblContextualHelp->setText("This folder that will house your LePlatz project file. This should have write "
                                   "permissions enabled for the intended users.");
}

void NewProject::displayArtFolderHelp()
{
    ui->lblContextualHelp->setText("To enable default settings, this folder should contain:\n"
                                   "./Slices/\t./tiles" + ui->cboImageFormat->currentText() +
                                   "\n./maps" + ui->cboImageFormat->currentText() +
                                   "\t./animations" + ui->cboImageFormat->currentText());
}

void NewProject::displaySrcFolderHelp()
{
    QString projectName = ui->leProjectName->text(), xmlStr = ".xml";

    if (projectName.endsWith(xmlStr))
        projectName.chop(xmlStr.length());
    ui->lblContextualHelp->setText("To enable default settings, this folder should contain:\n"
                                   "./<Required .c & .h files>\t./data/\n"
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
                                   "./default/Makefile\t./default/" + projectName + ".hex");
#else
                                    "./default/Makefile\t\t./default/" + projectName + ".hex");
#endif
}

void NewProject::displayVideoModeHelp()
{
    ui->lblContextualHelp->setText("Resolution represents vram array horizontally, and screen tiles vertically (both converted to pixels). "
                                   "This is because only horizontal scrolling is currently supported.");
}

void NewProject::displayOverlayLinesHelp()
{
    ui->lblContextualHelp->setText("Overlay lines are static horizontal sections of the screen used to display status information. "
                                   "Increasing this value will reduce the height of your slices.");
}

QString NewProject::folderFileDialog(const QString &initPath, const QString &title)
{
    QDir dir(initPath);

    if (!dir.exists())
        dir.setPath(QDir::currentPath());
    return QFileDialog::getExistingDirectory(this, title, dir.absolutePath());
}

void NewProject::projectFolderFileDialog()
{
    QString path = folderFileDialog(ui->leProjectFolder->text(), "LePlatz Project Path");

    if (!path.isEmpty())
        ui->leProjectFolder->setText(path);
}

void NewProject::artFolderFileDialog()
{
    QString path;

    if (!ui->leArtFolder->text().isEmpty())
        path = folderFileDialog(ui->leArtFolder->text(), "LePlatz Art Path");
    else
        path = folderFileDialog(ui->leProjectFolder->text(), "LePlatz Art Path");

    if (!path.isEmpty())
        ui->leArtFolder->setText(path);
}

void NewProject::srcFolderFileDialog()
{
    QString path;

    if (!ui->leSrcFolder->text().isEmpty())
        path = folderFileDialog(ui->leSrcFolder->text(), "LePlatz Source Path");
    else
        path = folderFileDialog(ui->leProjectFolder->text(), "LePlatz Source Path");

    if (!path.isEmpty())
        ui->leSrcFolder->setText(path);
}

bool NewProject::validateInput()
{
    QDir dir(ui->leProjectFolder->text());

    if (ui->leProjectFolder->text().isEmpty() || !dir.exists()) {
        ui->lblContextualHelp->setText("Valid Project Folder required.");
        return false;
    }
    QString projectName = ui->leProjectName->text().trimmed(), xml = ".xml";

    if (projectName.endsWith(xml))
        projectName.chop(xml.length());
    // Some of the Windows OS reserved words probably don't matter due to adding .xml suffix
    // but in case that is removed for whatever reason, better to avoid them
    QRegExp regexp("(^(?=(PRN|AUX|CLOCK\\$|NUL|CON|COM\\d{1,2}|LPT\\d{1,2})?$)|[\\/:*?\"<>|])", Qt::CaseInsensitive);

    if (projectName.contains(regexp)) {
        ui->lblContextualHelp->setText("Project Name must be at least one character in length and cannot contain any of the following characters: "
                                       " \\ / : * ? \" < > | or common OS reserved words.");
        return false;
    }
    return true;
}

void NewProject::createNewProject()
{
    if (!validateInput())
        return;
    QString projectName = ui->leProjectName->text().trimmed();

    if (!projectName.endsWith(".xml"))
        projectName += ".xml";
    QString projectPath = ui->leProjectFolder->text() + "/" + projectName;

    if (QDir::isRelativePath(projectPath)) {
        ui->lblContextualHelp->setText("Project Path must be absolute.");
        return;
    }

    if (QFile::exists(projectPath)) {
        QMessageBox msgbox("Overwrite existing file?", "A file with the name " + projectName + " already exists in the specified directory. "
                           "Overwrite this file?", QMessageBox::Question, QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::NoButton, this);
        int ret = msgbox.exec();

        if (ret == QMessageBox::Cancel)
            return;
    }

    settings->setVideoMode(ui->cboVideoMode->currentIndex()+2);

    QSize sliceSize = ui->cboVideoMode->itemData(ui->cboVideoMode->currentIndex(), Qt::UserRole).toSize();

    sliceSize.setHeight(sliceSize.height() - 8*ui->spbOverlayLines->value());

    settings->setProjectPath(ui->leProjectFolder->text() + "/" + projectName);
    settings->setImageFormat(ui->cboImageFormat->currentText());
    settings->setArtFolder(ui->leArtFolder->text());
    settings->setRelativeSrcFolder(ui->leSrcFolder->text());
    settings->setSliceSize(sliceSize);
    accept();
}

NewProject::~NewProject()
{
    delete ui;
}

