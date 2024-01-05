// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/speciesAngleModel.h"
#include "gui/models/speciesBondModel.h"
#include "gui/models/speciesImproperModel.h"
#include "gui/models/speciesTorsionModel.h"
#include "gui/ui_intramolecularTermsDialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;
class DissolveWindow;

class IntramolecularTermsDialog : public QDialog
{
    Q_OBJECT

    public:
    IntramolecularTermsDialog(QWidget *parent, Dissolve &dissolve, Species *species, DissolveWindow *dissolveWindow);
    ~IntramolecularTermsDialog() = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::IntramolecularTermsDialog ui_;

    private:
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * Models
     */
    private:
    SpeciesBondModel bonds_;
    SpeciesAngleModel angles_;
    SpeciesTorsionModel torsions_;
    SpeciesImproperModel impropers_;

    private slots:
    // Finish
    void on_OKButton_clicked(bool checked);
};
