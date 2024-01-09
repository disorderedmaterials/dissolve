// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/scaleChargesDialog.h"

ScaleChargesDialog::ScaleChargesDialog(QWidget *parent) { ui_.setupUi(this); }
ScaleChargesDialog::~ScaleChargesDialog() {}

double ScaleChargesDialog::scaleValue() const { return ui_.ScaleSpinBox->value(); }

void ScaleChargesDialog::on_CancelButton_clicked(bool checked) { reject(); }
void ScaleChargesDialog::on_ScaleButton_clicked(bool checked)
{
    scale_ = true;
    accept();
}
void ScaleChargesDialog::on_ScaleToButton_clicked(bool checked)
{
    scaleTo_ = true;
    accept();
}
