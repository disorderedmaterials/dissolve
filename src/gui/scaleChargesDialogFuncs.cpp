// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/scaleChargesDialog.h"

ScaleChargesDialog::ScaleChargesDialog(QWidget *parent) {
    QQuickWidget *view = new QQuickWidget(QUrl("qrc:/dialogs/qml/ScaleChargesDialog.qml"));
    
    connect(this, SIGNAL(cancel()), this, SLOT(cancelClicked()));
    connect(this, SIGNAL(scale()), this, SLOT(cancelClicked()));
    connect(this, SIGNAL(scaleTo()), this, SLOT(cancelClicked()));
}
ScaleChargesDialog::~ScaleChargesDialog() {}

double ScaleChargesDialog::value() const { return currentValue_; }

void ScaleChargesDialog::cancelClicked() { reject(); }
void ScaleChargesDialog::scaleClicked()
{
    scale_ = true;
    accept();
}
void ScaleChargesDialog::scaleToClicked()
{
    scaleTo_ = true;
    accept();
}
