// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/modifyChargesDialog.h"
#include "gui/models/modifyChargesModel.h"
#include <QHBoxLayout>
#include <QObject>
#include <QQmlContext>
#include <QQuickItem>
#include <QString>
#include <string>

ModifyChargesDialog::ModifyChargesDialog(QWidget *parent, ModifyChargesModel *dialogModel, ModifyChargesModel::ModifyBy modify)
    : QDialog(parent)
{
    view_ = new QQuickWidget(this);
    view_->rootContext()->setContextProperty("modify", modify);
    view_->rootContext()->setContextProperty("dialogModel", dialogModel);
    view_->setSource(QUrl("qrc:/dialogs/qml/modifyCharges/Loader.qml"));

    view_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    auto *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(view_);
    setLayout(topLeftLayout);

    QObject::connect(dialogModel, SIGNAL(cancelSelection()), this, SLOT(reject()));
    QObject::connect(dialogModel, SIGNAL(acceptSelection()), this, SLOT(accept()));
}