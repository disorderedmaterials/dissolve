// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/scaleChargesDialog.h"
#include <QQuickWidget>

ScaleChargesDialog::ScaleChargesDialog(QWidget *parent) {
    QQuickWidget *view = new QQuickWidget(QUrl("qrc:/dialogs/qml/ScaleChargesDialog.qml"), this);

    QObject::connect(this, SIGNAL(cancelSelection()), this, SLOT(reject()));
    QObject::connect(this, SIGNAL(acceptSelection()), this, SLOT(accept()));
}

ScaleChargesDialog::~ScaleChargesDialog() {}

double ScaleChargesDialog::value() const { return currentValue_; }

void ScaleChargesDialog::updateValue(double newVal) { currentValue_ = newVal; }

void ScaleChargesDialog::setOption(int option) {
    switch(option){
        case 1:
            scale_=true;
            scaleTo_=false;
            break;
        case 2:
            scale_=false;
            scaleTo_=true;
            break;
        default:
            break;
    }
}