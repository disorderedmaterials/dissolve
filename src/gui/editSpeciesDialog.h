// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_editSpeciesDialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;

// Add Forcefield Terms Dialog
class EditSpeciesDialog : public QDialog
{
    Q_OBJECT

    public:
    EditSpeciesDialog(QWidget *parent, Species *sp);
    ~EditSpeciesDialog();

    private:
    // Target Species
    Species *species_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::EditSpeciesDialog ui_;

    private slots:
    void on_CancelButton_clicked(bool checked);
    void on_OKButton_clicked(bool checked);
    void dataModified();

    public:
    // Run the dialog, returning whether it was accepted
    bool editSpecies();
};
