
#include <QStringList>
#include "FindReplace.h"
#include "ui_FindReplace.h"

FindReplace::FindReplace(PlatzDataModel *model, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::FindReplace), model(model)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->rbtnApplyAll->setChecked(true);
    ui->spbRangeLower->setDisabled(true);
    ui->spbRangeUpper->setDisabled(true);
    ui->lblReplaceResult->setText("");
    ui->cboCategory->addItems(QStringList() << "Trigger Ids" << "Platform Clear Tiles" << "Mutable Class Ids" << "Mutable Bg Ids");
    ui->cboCategory->setItemData(0, QVariant((int)WorldItem::TriggerStrings), Qt::UserRole);
    ui->cboCategory->setItemData(1, QVariant((int)WorldItem::ClrTileStrings), Qt::UserRole);
    ui->cboCategory->setItemData(2, QVariant((int)WorldItem::MutClassIdStrings), Qt::UserRole);
    ui->cboCategory->setItemData(3, QVariant((int)WorldItem::MutIdStrings), Qt::UserRole);

    int max = (model && model->root()) ? model->root()->childCount()-1 : 0;
    ui->spbRangeLower->setRange(0, max);
    ui->spbRangeUpper->setRange(0, max);

    // Connections
    connect(ui->cboCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(updateReplaceThisCombo(int)));
    connect(ui->cboCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(updateReplaceWithCombo(int)));
    connect(ui->rbtnApplyRange, SIGNAL(toggled(bool)), ui->spbRangeLower, SLOT(setEnabled(bool)));
    connect(ui->rbtnApplyRange, SIGNAL(toggled(bool)), ui->spbRangeUpper, SLOT(setEnabled(bool)));
    connect(ui->pbReplace, SIGNAL(clicked()), this, SLOT(replace()));
    connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(close()));
    updateReplaceThisCombo(ui->cboCategory->currentIndex());
    updateReplaceWithCombo(ui->cboCategory->currentIndex());
}

void FindReplace::updateReplaceThisCombo(int index)
{
    populateReplaceThisCombo(static_cast<WorldItem::StringDataType>(ui->cboCategory->itemData(index).toInt()));
}

void FindReplace::populateReplaceThisCombo(const WorldItem::StringDataType &type)
{
    if (!model)
        return;
    QStringList data;

    if (ui->rbtnApplyAll) {
        model->retrieveStringData(data, type);
    } else {
        int from = ui->spbRangeLower->value(), to = ui->spbRangeUpper->value();
        model->retrieveStringData(data, type, from, to);
    }
    ui->cboReplaceThis->clear();
    ui->cboReplaceThis->addItems(data);
}

void FindReplace::updateReplaceWithCombo(int index)
{
    populateReplaceWithCombo(static_cast<WorldItem::StringDataType>(ui->cboCategory->itemData(index).toInt()));
}

void FindReplace::populateReplaceWithCombo(const WorldItem::StringDataType &type)
{
    ui->cboReplaceWith->clear();

    switch (type) {
        case WorldItem::TriggerStrings: ui->cboReplaceWith->addItems(WorldItem::triggerIds); break;
        case WorldItem::ClrTileStrings: ui->cboReplaceWith->addItems(WorldItem::platClrTileIds); break;
        case WorldItem::MutClassIdStrings: ui->cboReplaceWith->addItems(WorldItem::mutableClassIds); break;
        case WorldItem::MutIdStrings: ui->cboReplaceWith->addItems(WorldItem::mutableIds); break;
        default: break;
    }
}

void FindReplace::replace()
{
    if (!model)
        return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    WorldItem::StringDataType type = static_cast<WorldItem::StringDataType>(ui->cboCategory->itemData(ui->cboCategory->currentIndex()).toInt());
    int from = (ui->spbRangeLower->isEnabled())?ui->spbRangeLower->value():-1;
    int to = (ui->spbRangeUpper->isEnabled())?ui->spbRangeUpper->value():-1;
    int numReplaced = model->replaceStringData(QStringList() << ui->cboReplaceThis->currentText() << ui->cboReplaceWith->currentText(), type, from, to);

    if (numReplaced)
        populateReplaceThisCombo(type);
    QApplication::restoreOverrideCursor();
    ui->lblReplaceResult->setText("Total replacements: " + QString::number(numReplaced));
}

FindReplace::~FindReplace()
{
    delete ui;
}
