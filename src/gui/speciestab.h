// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "gui/maintab.h"
#include "gui/models/speciesAngleModel.h"
#include "gui/models/speciesAtomModel.h"
#include "gui/models/speciesBondModel.h"
#include "gui/models/speciesImproperModel.h"
#include "gui/models/speciesIsoModel.h"
#include "gui/models/speciesSiteModel.h"
#include "gui/models/speciesTorsionModel.h"
#include "gui/ui_speciestab.h"

// Forward Declarations
class AtomType;
class Isotopologue;
class Species;

// Species Tab
class SpeciesTab : public QWidget, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    SpeciesTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
               Species *species);
    ~SpeciesTab() = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SpeciesTab ui_;
    SpeciesAtomModel atoms_;
    SpeciesAngleModel angles_;
    SpeciesBondModel bonds_;
    SpeciesTorsionModel torsions_;
    SpeciesImproperModel impropers_;
    SpeciesIsoModel isos_;
    SpeciesSiteModel sites_;

    public slots:
    // Update controls in tab
    void updateControls();
    // Disable sensitive controls within tab
    void disableSensitiveControls();
    // Enable sensitive controls within tab
    void enableSensitiveControls();

    /*
     * MainTab Reimplementations
     */
    public:
    // Return tab type
    MainTab::TabType type() const;
    // Raise suitable dialog for entering / checking new tab name
    QString getNewTitle(bool &ok);
    // Return whether the title of the tab can be changed
    bool canChangeTitle() const;
    // Return whether the tab can be closed (after any necessary user querying, etc.)
    bool canClose() const;

    /*
     * Species Target
     */
    private:
    // Species data to display
    Species *species_;
    // Version of the Species at which the UI was last updated
    int speciesVersion_{-1};

    public:
    // Return displayed Species
    Species *species() const;

    /*
     * Widget Functions - Geometry
     */
    private:
    // Return valid AtomType names for specified model index in the SpeciesAtomTable
    std::vector<std::string> validAtomTypeNames(const QModelIndex &index);

    private slots:
    // Atom table item changed
    void atomTableDataChanged(const QModelIndex &, const QModelIndex &);
    // Respond to changes in the Atom Table
    void updateAtomTableSelection();
    // Respond to selection in the underlying model
    void updateUnderlyingAtomSelection();

    public slots:
    // Update total charges
    void updateTotalCharges();
    // Update Geometry tablss
    void updateGeometryTables();

    /*
     * Widget Functions - Isotopologues
     */
    private:
    // Return currently-selected Isotopologue
    Isotopologue *currentIsotopologue();

    private slots:
    void on_IsotopologueRemoveButton_clicked(bool checked);
    void on_IsotopologueGenerateButton_clicked(bool checked);
    void on_IsotopologueExpandAllButton_clicked(bool checked);
    void on_IsotopologueCollapseAllButton_clicked(bool checked);

    public slots:
    // Update Isotopologues tab
    void updateIsotopologuesTab();

    /*
     * Widget Functions - Sites
     */
    private:
    // Return currently-selected SpeciesSite
    SpeciesSite *currentSite();

    private slots:
    void setCurrentSiteFromViewer();
    void on_SiteAddButton_clicked(bool checked);
    void on_SiteRemoveButton_clicked(bool checked);
    void siteSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void on_SiteOriginMassWeightedCheck_clicked(bool checked);

    public slots:
    // Update sites tab
    void updateSitesTab();
};
