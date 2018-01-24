/********************************************************************************
** Form generated from reading UI file 'PropertyWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROPERTYWIDGET_H
#define UI_PROPERTYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PropertyWidget
{
public:
    QWidget *dockWidgetContents;
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *label_2;
    QComboBox *comboBoxEdgeType;
    QSpinBox *spinBoxNumLanes;
    QCheckBox *checkBoxOneWay;
    QPushButton *pushButtonApply;

    void setupUi(QDockWidget *PropertyWidget)
    {
        if (PropertyWidget->objectName().isEmpty())
            PropertyWidget->setObjectName(QStringLiteral("PropertyWidget"));
        PropertyWidget->resize(213, 196);
        PropertyWidget->setMinimumSize(QSize(213, 196));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 191, 151));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 47, 21));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 50, 47, 21));
        comboBoxEdgeType = new QComboBox(groupBox);
        comboBoxEdgeType->setObjectName(QStringLiteral("comboBoxEdgeType"));
        comboBoxEdgeType->setGeometry(QRect(60, 20, 121, 22));
        spinBoxNumLanes = new QSpinBox(groupBox);
        spinBoxNumLanes->setObjectName(QStringLiteral("spinBoxNumLanes"));
        spinBoxNumLanes->setGeometry(QRect(60, 50, 71, 22));
        checkBoxOneWay = new QCheckBox(groupBox);
        checkBoxOneWay->setObjectName(QStringLiteral("checkBoxOneWay"));
        checkBoxOneWay->setGeometry(QRect(10, 80, 70, 17));
        pushButtonApply = new QPushButton(groupBox);
        pushButtonApply->setObjectName(QStringLiteral("pushButtonApply"));
        pushButtonApply->setGeometry(QRect(40, 110, 121, 31));
        PropertyWidget->setWidget(dockWidgetContents);

        retranslateUi(PropertyWidget);

        QMetaObject::connectSlotsByName(PropertyWidget);
    } // setupUi

    void retranslateUi(QDockWidget *PropertyWidget)
    {
        dockWidgetContents->setWindowTitle(QApplication::translate("PropertyWidget", "PropertyWidget", 0));
        groupBox->setTitle(QApplication::translate("PropertyWidget", "Edge", 0));
        label->setText(QApplication::translate("PropertyWidget", "Type:", 0));
        label_2->setText(QApplication::translate("PropertyWidget", "#lanes:", 0));
        checkBoxOneWay->setText(QApplication::translate("PropertyWidget", "One way", 0));
        pushButtonApply->setText(QApplication::translate("PropertyWidget", "Apply", 0));
        Q_UNUSED(PropertyWidget);
    } // retranslateUi

};

namespace Ui {
    class PropertyWidget: public Ui_PropertyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROPERTYWIDGET_H
