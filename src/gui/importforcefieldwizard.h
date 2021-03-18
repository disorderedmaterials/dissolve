// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_importforcefieldwizard.h"
#include "gui/models/xmlAngleModel.h"
#include "gui/models/xmlAtomModel.h"
#include "gui/models/xmlBondModel.h"
#include "gui/models/xmlImproperModel.h"
#include "gui/models/xmlTorsionModel.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class ForcefieldWizard;

// Import ForcefieldWizard Dialog
class ImportForcefieldWizard : public QDialog
{
    Q_OBJECT

    public:
    ImportForcefieldWizard(QWidget *parent, Dissolve &mainDissolveInstance);
    ~ImportForcefieldWizard(){};

    private:
    // Main form declaration
    Ui::ImportForcefieldWizardDialog ui_;
    XmlAtomModel atoms_;
    XmlBondModel bonds_;
    XmlAngleModel angles_;
    XmlTorsionModel torsions_;
    XmlImproperModel impropers_;

    private slots:
    void updateNavButtons();
    void nextStack();
    void prevStack();
    void xmlFileDialog();
    void xmlString(QString);

    public:
    // Reset, ready for adding a new ForcefieldWizard
    void reset(){};
    /* // Copy imported ForcefieldWizard over to the specified Dissolve object */
    /* ForcefieldWizard *importForcefieldWizard(Dissolve &dissolve); */
};
