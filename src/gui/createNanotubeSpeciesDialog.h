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
    // Chirality parameters
    int n_{0}, m_{0};
    // Sheet width (A) and height (c)
    double A_{0.0}, c_{0.0};
    // Representative unit length
    double a0_{0.0};
    // Principal vectors
    Vec3<double> va1_, va2_;
    // Angle between principal vector va1 and the sheet vector
    double alpha_{0.0};
    // Radius of resulting tube
    double radius_{0.0};
    // Delta along y between helices
    double dy_{0.0};
    // Number of helices required
    int H_{0};
    // Default size of unit cell for sheet
    const double sheetZ_{10.0};

    private:
    // Calculate parameters
    void calculateParameters();
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
    // Sheet Definition
    void on_NSpin_valueChanged(int value);
    void on_MSpin_valueChanged(int value);
    void on_ElementAButton_clicked(bool checked);
    void on_ElementBButton_clicked(bool checked);
    void on_BondLengthSpin_valueChanged(double value);
    // Sheet Transforms
    void on_RollUpCheck_clicked(bool checked);
    // Output Options
    void on_PeriodicRadio_clicked(bool checked);
    void on_NonPeriodicRadio_clicked(bool checked);
    void on_PseudoPeriodicRadio_clicked(bool checked);
    // Dialog
    void on_OKButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);
};
