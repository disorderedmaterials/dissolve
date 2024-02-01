// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/modifyChargesModel.h"
#include "gui/modifyChargesDialog.h"
#include <QObject>
#include <QHBoxLayout>
#include <QQmlContext>
#include <QQuickItem>

ModifyChargesDialog::ModifyChargesDialog(QWidget *parent, ModifyChargesModel *dialogModel, ModifyChargesModel::Modify modify) : QDialog(parent)
{
    view_ = new QQuickWidget(this);
    view_->rootContext()->setContextProperty("modify", modify);
    view_->rootContext()->setContextProperty("dialogModel", dialogModel);
    view_->setSource(QUrl("qrc:/dialogs/qml/ModifyChargesDialog.qml"));

    auto root = view_->rootObject();
    auto spinBox = root->findChild<QObject*>("spinBox");

    initializeSpinBox(spinBox);

    view_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(view_);
    setLayout(topLeftLayout);

    QObject::connect(dialogModel, SIGNAL(cancelSelection()), this, SLOT(reject()));
    QObject::connect(dialogModel, SIGNAL(acceptSelection()), this, SLOT(accept()));
}

void ModifyChargesDialog::initializeSpinBox(QObject *spinBox, ModifyChargesModel::Modify modify) 
{    
    switch(modify) {
        case ModifyChargesModel::Scaling:
            spinBox->setProperty("value", dialogModel->scaleValue());
            break;
        case ModifyChargesModel::Smoothing:
            spinBox->setProperty("value", dialogModel->smoothValue());
            break;
        case ModifyChargesModel::ReduceSigFig:
            spinBox->setProperty("value", dialogModel->sigFigValue());
            break;
    }
}