// spdx-license-identifier: gpl-3.0-or-later
// copyright (c) 2023 team dissolve and contributors

#pragma once

#include "gui/ui_scaleChargesDialog.h"
#include <QDialog>

class ScaleChargesDialog : public QDialog
{
    Q_OBJECT

    public:
    ScaleChargesDialog(QWidget *parent);
    ~ScaleChargesDialog();
    bool scale_, scaleTo_;

    private:
    Ui::ScaleChargesDialog ui_;

    public:
    double scaleValue() const;

    private Q_SLOTS:
    void on_CancelButton_clicked(bool checked);
    void on_ScaleButton_clicked(bool checked);
    void on_ScaleToButton_clicked(bool checked);
};
