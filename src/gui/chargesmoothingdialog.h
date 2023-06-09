// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/ui_chargesmoothingdialog.h"
#include <QDialog>

class ChargeSmoothingDialog : public QDialog
{
    Q_OBJECT
    
    public:
    ChargeSmoothingDialog(QWidget *parent);
    ~ChargeSmoothingDialog() = default;

    private:
    // Main form declaration
    Ui::ChargeSmoothingDialog ui_;

    public:

    bool normalise() const;
    int normalisationTarget() const;
    bool roundOff() const;
    int significantFigures() const;
    /*
     * Slots
     */
    private slots:
    void on_CancelButton_clicked(bool checked);
    void on_OKButton_clicked(bool checked);
};

