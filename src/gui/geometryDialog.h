// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/models/speciesAngleModel.h"
#include "gui/models/speciesBondModel.h"
#include "gui/models/speciesImproperModel.h"
#include "gui/models/speciesTorsionModel.h"
#include "gui/ui_geometryDialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;

class GeometryDialog : public QDialog
{
    Q_OBJECT

    public:
    GeometryDialog(QWidget *parent, Dissolve &dissolve, Species *species);
    ~GeometryDialog() = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::GeometryDialog ui_;

    private:
    // Reference to Dissolve
    Dissolve &dissolve_;

    /*
     * Models
     */
    private:
    SpeciesAngleModel angles_;
    SpeciesBondModel bonds_;
    SpeciesTorsionModel torsions_;
    SpeciesImproperModel impropers_;

    private:
    bool modified_{false};

    public:
    bool modified();

    private slots:
    void on_OKButton_clicked(bool checked);
    void setModified();
};
