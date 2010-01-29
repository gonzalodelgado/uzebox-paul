/********************************************************************************
** Form generated from reading UI file 'FindReplace.ui'
**
** Created: Sat 30. Jan 02:48:23 2010
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

QT_BEGIN_NAMESPACE

class Ui_FindReplace
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame;
    QFormLayout *formLayout;
    QLabel *lblCategory;
    QComboBox *cboCategory;
    QLabel *lblReplaceThis;
    QComboBox *cboReplaceThis;
    QLabel *lblReplaceWith;
    QComboBox *cboReplaceWith;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *rbtnApplyAll;
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
        FindReplace->resize(475, 228);
        FindReplace->setMinimumSize(QSize(475, 228));
        FindReplace->setMaximumSize(QSize(548, 228));
        horizontalLayout_2 = new QHBoxLayout(FindReplace);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        frame = new QFrame(FindReplace);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        frame->setLineWidth(0);
        formLayout = new QFormLayout(frame);
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

        verticalSpacer = new QSpacerItem(357, 8, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(3, QFormLayout::FieldRole, verticalSpacer);

        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        rbtnApplyAll = new QRadioButton(groupBox);
        rbtnApplyAll->setObjectName(QString::fromUtf8("rbtnApplyAll"));

        verticalLayout->addWidget(rbtnApplyAll);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        rbtnApplyRange = new QRadioButton(groupBox);
        rbtnApplyRange->setObjectName(QString::fromUtf8("rbtnApplyRange"));

        horizontalLayout->addWidget(rbtnApplyRange);

        spbRangeLower = new QSpinBox(groupBox);
        spbRangeLower->setObjectName(QString::fromUtf8("spbRangeLower"));

        horizontalLayout->addWidget(spbRangeLower);

        lblRangeTo = new QLabel(groupBox);
        lblRangeTo->setObjectName(QString::fromUtf8("lblRangeTo"));

        horizontalLayout->addWidget(lblRangeTo);

        spbRangeUpper = new QSpinBox(groupBox);
        spbRangeUpper->setObjectName(QString::fromUtf8("spbRangeUpper"));

        horizontalLayout->addWidget(spbRangeUpper);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        formLayout->setWidget(4, QFormLayout::SpanningRole, groupBox);


        verticalLayout_3->addWidget(frame);

        lblReplaceResult = new QLabel(FindReplace);
        lblReplaceResult->setObjectName(QString::fromUtf8("lblReplaceResult"));

        verticalLayout_3->addWidget(lblReplaceResult);


        horizontalLayout_2->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        pbReplace = new QPushButton(FindReplace);
        pbReplace->setObjectName(QString::fromUtf8("pbReplace"));

        verticalLayout_2->addWidget(pbReplace);

        pbClose = new QPushButton(FindReplace);
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
