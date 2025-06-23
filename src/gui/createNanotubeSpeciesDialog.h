// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/lock.h"
#include "gui/models/cifAssemblyModel.h"
#include "gui/selectElementDialog.h"
#include "gui/ui_createNanotubeSpeciesDialog.h"
#include "gui/wizard.h"
#include "io/import/cif.h"
#include "main/dissolve.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;

// Create Nanotube Species Dialog
class CreateNanotubeSpeciesDialog : public QDialog
{
    Q_OBJECT

    public:
    CreateNanotubeSpeciesDialog(QWidget *parent, Dissolve &dissolve);
    ~CreateNanotubeSpeciesDialog() = default;

    private:
    // Main form declaration
    Ui::CreateNanotubeSpeciesDialog ui_;
    // Temporary core data
    CoreData temporaryCoreData_;

    /*
     * Data
     */
    private:
    // Main Dissolve object
    Dissolve &dissolve_;
    // Elements in nanotube
    Elements::Element zA_{Elements::Element::C}, zB_{Elements::Element::N};
    // Generated nanotube species
    Species species_;

    private:
    // Plot an AB atom layer
    void plotLayer(double z, double tubeRadius, double radialStep, double radialOffset);
    // Regenerate species
    void regenerate();

    /*
     * UI
     */
    private:
    // Widget update lock
    Lock widgetsUpdating_;
    // Element selection dialog
    SelectElementDialog selectElementDialog_;

    private:
    // Update all controls
    void updateWidgets();

    private Q_SLOTS:
    void on_AxialRingLengthSpin_valueChanged(int value);
    void on_RadialRingSizeSpin_valueChanged(int value);
    void on_ElementAButton_clicked(bool checked);
    void on_ElementBButton_clicked(bool checked);
    // Dialog
    void on_OKButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);
};
