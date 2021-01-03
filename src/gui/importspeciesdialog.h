// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_importspeciesdialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;

// Import Species Dialog
class ImportSpeciesDialog : public QDialog
{
    Q_OBJECT

    public:
    ImportSpeciesDialog(QWidget *parent, const Dissolve &mainDissolveInstance);
    ~ImportSpeciesDialog();

    private:
    // Main form declaration
    Ui::ImportSpeciesDialog ui_;

    public:
    // Reset, ready for adding a new Species
    void reset();
    // Copy imported Species over to the specified Dissolve object
    Species *importSpecies(Dissolve &dissolve);
};
