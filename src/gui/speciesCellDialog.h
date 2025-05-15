// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "gui/widgets/exponentialSpin.h"
#include "classes/species.h"
#include "gui/ui_speciesCellDialog.h"
#include <QDialog>

// Species Widget
class SpeciesCellDialog : public QDialog
{
    Q_OBJECT

    public:
    SpeciesCellDialog(QWidget *parent, Species *sp);
    ~SpeciesCellDialog() = default;

    private:
    // Main Dissolve pointer
    Ui::SpeciesCellDialog ui_;
    QDialog *view_{nullptr};

    // Species
    Species *sp_;

    private Q_SLOTS:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

};