#ifndef FINDREPLACE_H
#define FINDREPLACE_H

#include <QDialog>
#include <PlatzDataModel.h>
#include <WorldItem.h>

namespace Ui {
    class FindReplace;
}

class FindReplace : public QDialog
{
    Q_OBJECT

public:
    FindReplace(PlatzDataModel *model, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    ~FindReplace();
private slots:
    void replace();
    void updateReplaceThisCombo(int index);
    void updateReplaceWithCombo(int index);
    void populateReplaceThisCombo(const WorldItem::StringDataType &type);
    void populateReplaceWithCombo(const WorldItem::StringDataType &type);
private:
    Ui::FindReplace *ui;
    PlatzDataModel *model;
};

#endif // FINDREPLACE_H
