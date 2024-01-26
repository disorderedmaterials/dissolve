// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/scaleChargesDialogModel.h"
#include "gui/scaleChargesDialog.h"
#include <QHBoxLayout>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>

ScaleChargesDialog::ScaleChargesDialog(QWidget *parent) : QDialog(parent)
{
    QQuickWidget *view = new QQuickWidget(QUrl("qrc:/dialogs/qml/ScaleChargesDialog.qml"), this);

    auto root = view->rootObject();
    model = root->findChild<ScaleChargesDialogModel *>("dialogModel");

    view->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(view);
    setLayout(topLeftLayout);

    QObject::connect(model, SIGNAL(cancelSelection()), this, SLOT(reject()));
    QObject::connect(model, SIGNAL(acceptSelection()), this, SLOT(accept()));
}