// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_addforcefieldtermsdialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;

// Add Forcefield Terms Dialog
class AddForcefieldTermsDialog : public QDialog
{
    Q_OBJECT

    public:
    AddForcefieldTermsDialog(QWidget *parent, const Dissolve &mainDissolveInstance);
    ~AddForcefieldTermsDialog();

    private:
    // Main form declaration
    Ui::AddForcefieldTermsDialog ui_;

    public:
    // Reset, ready for adding a new Configuration
    void reset();
    // Set target Species that we are acquiring forcefield terms for
    void setTargetSpecies(Species *sp);
    // Apply Forcefield terms to the targetted Species within the specified Dissolve object
    bool applyForcefieldTerms(Dissolve &dissolve);
};
