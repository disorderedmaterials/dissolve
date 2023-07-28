// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/rFactorAddRangeDialog.h"

RFactorAddRangeDialog::RFactorAddRangeDialog(QWidget *parent)
{
    ui_.setupUi(this);
    table_ = ui_.RFactorRangeTable;
}

RFactorAddRangeDialog::~RFactorAddRangeDialog() {}

void RFactorAddRangeDialog::on_OkButton_clicked(bool checked) { reject(); }

void RFactorAddRangeDialog::on_CancelButton_clicked(bool checked) { reject(); }

void RFactorAddRangeDialog::on_RangeAddButton_clicked(bool checked) { table_->insertRow(table_->rowCount()); }

void RFactorAddRangeDialog::on_RangeRemoveButton_clicked(bool checked) { table_->removeRow(table_->currentRow()); }
