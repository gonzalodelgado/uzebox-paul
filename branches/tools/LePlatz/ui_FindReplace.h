/********************************************************************************
** Form generated from reading UI file 'FindReplace.ui'
**
** Created: Sun 31. Jan 05:11:44 2010
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDREPLACE_H
#define UI_FINDREPLACE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FindReplace
{
public:
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *lblCategory;
    QComboBox *cboCategory;
    QLabel *lblReplaceThis;
    QComboBox *cboReplaceThis;
    QLabel *lblReplaceWith;
    QComboBox *cboReplaceWith;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QRadioButton *rbtnApplyAll;
    QWidget *widget1;
    QHBoxLayout *horizontalLayout;
    QRadioButton *rbtnApplyRange;
    QSpinBox *spbRangeLower;
    QLabel *lblRangeTo;
    QSpinBox *spbRangeUpper;
    QSpacerItem *horizontalSpacer;
    QLabel *lblReplaceResult;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pbReplace;
    QPushButton *pbClose;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QDialog *FindReplace)
    {
        if (FindReplace->objectName().isEmpty())
            FindReplace->setObjectName(QString::fromUtf8("FindReplace"));
        FindReplace->resize(475, 300);
        FindReplace->setMinimumSize(QSize(475, 300));
        FindReplace->setMaximumSize(QSize(475, 300));
        widget = new QWidget(FindReplace);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 9, 464, 267));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        frame = new QFrame(widget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        frame->setLineWidth(0);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        lblCategory = new QLabel(frame);
        lblCategory->setObjectName(QString::fromUtf8("lblCategory"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lblCategory);

        cboCategory = new QComboBox(frame);
        cboCategory->setObjectName(QString::fromUtf8("cboCategory"));

        formLayout->setWidget(0, QFormLayout::FieldRole, cboCategory);

        lblReplaceThis = new QLabel(frame);
        lblReplaceThis->setObjectName(QString::fromUtf8("lblReplaceThis"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lblReplaceThis);

        cboReplaceThis = new QComboBox(frame);
        cboReplaceThis->setObjectName(QString::fromUtf8("cboReplaceThis"));

        formLayout->setWidget(1, QFormLayout::FieldRole, cboReplaceThis);

        lblReplaceWith = new QLabel(frame);
        lblReplaceWith->setObjectName(QString::fromUtf8("lblReplaceWith"));

        formLayout->setWidget(2, QFormLayout::LabelRole, lblReplaceWith);

        cboReplaceWith = new QComboBox(frame);
        cboReplaceWith->setObjectName(QString::fromUtf8("cboReplaceWith"));

        formLayout->setWidget(2, QFormLayout::FieldRole, cboReplaceWith);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(357, 8, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setMinimumSize(QSize(0, 112));
        rbtnApplyAll = new QRadioButton(groupBox);
        rbtnApplyAll->setObjectName(QString::fromUtf8("rbtnApplyAll"));
        rbtnApplyAll->setGeometry(QRect(10, 23, 71, 31));
        rbtnApplyAll->setMinimumSize(QSize(71, 31));
        widget1 = new QWidget(groupBox);
        widget1->setObjectName(QString::fromUtf8("widget1"));
        widget1->setGeometry(QRect(11, 68, 321, 33));
        horizontalLayout = new QHBoxLayout(widget1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        rbtnApplyRange = new QRadioButton(widget1);
        rbtnApplyRange->setObjectName(QString::fromUtf8("rbtnApplyRange"));
        rbtnApplyRange->setMinimumSize(QSize(71, 31));

        horizontalLayout->addWidget(rbtnApplyRange);

        spbRangeLower = new QSpinBox(widget1);
        spbRangeLower->setObjectName(QString::fromUtf8("spbRangeLower"));
        spbRangeLower->setMinimumSize(QSize(51, 21));

        horizontalLayout->addWidget(spbRangeLower);

        lblRangeTo = new QLabel(widget1);
        lblRangeTo->setObjectName(QString::fromUtf8("lblRangeTo"));
        lblRangeTo->setMinimumSize(QSize(25, 31));
        lblRangeTo->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(lblRangeTo);

        spbRangeUpper = new QSpinBox(widget1);
        spbRangeUpper->setObjectName(QString::fromUtf8("spbRangeUpper"));
        spbRangeUpper->setMinimumSize(QSize(51, 21));

        horizontalLayout->addWidget(spbRangeUpper);

        horizontalSpacer = new QSpacerItem(17, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(groupBox);


        verticalLayout_3->addWidget(frame);

        lblReplaceResult = new QLabel(widget);
        lblReplaceResult->setObjectName(QString::fromUtf8("lblReplaceResult"));
        lblReplaceResult->setMinimumSize(QSize(0, 31));

        verticalLayout_3->addWidget(lblReplaceResult);


        horizontalLayout_2->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        pbReplace = new QPushButton(widget);
        pbReplace->setObjectName(QString::fromUtf8("pbReplace"));

        verticalLayout_2->addWidget(pbReplace);

        pbClose = new QPushButton(widget);
        pbClose->setObjectName(QString::fromUtf8("pbClose"));

        verticalLayout_2->addWidget(pbClose);

        verticalSpacer_2 = new QSpacerItem(20, 88, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        horizontalLayout_2->addLayout(verticalLayout_2);


        retranslateUi(FindReplace);

        QMetaObject::connectSlotsByName(FindReplace);
    } // setupUi

    void retranslateUi(QDialog *FindReplace)
    {
        FindReplace->setWindowTitle(QApplication::translate("FindReplace", "Find & Replace Src Defines", 0, QApplication::UnicodeUTF8));
        lblCategory->setText(QApplication::translate("FindReplace", "Category:", 0, QApplication::UnicodeUTF8));
        lblReplaceThis->setText(QApplication::translate("FindReplace", "Replace this:", 0, QApplication::UnicodeUTF8));
        lblReplaceWith->setText(QApplication::translate("FindReplace", "With:", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("FindReplace", "Apply to slices", 0, QApplication::UnicodeUTF8));
        rbtnApplyAll->setText(QApplication::translate("FindReplace", "All", 0, QApplication::UnicodeUTF8));
        rbtnApplyRange->setText(QApplication::translate("FindReplace", "Range", 0, QApplication::UnicodeUTF8));
        lblRangeTo->setText(QApplication::translate("FindReplace", "to", 0, QApplication::UnicodeUTF8));
        lblReplaceResult->setText(QString());
        pbReplace->setText(QApplication::translate("FindReplace", "Replace", 0, QApplication::UnicodeUTF8));
        pbClose->setText(QApplication::translate("FindReplace", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FindReplace: public Ui_FindReplace {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDREPLACE_H
