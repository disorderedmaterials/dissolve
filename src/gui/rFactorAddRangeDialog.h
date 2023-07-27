// spdx-license-identifier: gpl-3.0-or-later
// copyright (c) 2023 team dissolve and contributors

#pragma once

#include "gui/ui_rFactorAddRangeDialog.h"
#include <QDialog>

class RFactorAddRangeDialog : public QDialog
{
    Q_OBJECT

    public:
    RFactorAddRangeDialog(QWidget *parent);
    ~RFactorAddRangeDialog();

    private:
    Ui::RFactorAddRangeDialog ui_;
    QTableWidget *table_;

    // public:
    // double scaleValue() const;

    private slots:
    void on_OkButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);
    void on_RangeAddButton_clicked(bool checked);
    void on_RangeRemoveButton_clicked(bool checked);
};
