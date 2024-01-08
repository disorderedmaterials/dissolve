// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/speciesAngle.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include "classes/speciesTorsion.h"
#include "gui/mainTab.h"
#include "gui/models/speciesAtomModel.h"
#include "gui/models/speciesIsoModel.h"
#include "gui/models/speciesSiteFilterProxy.h"
#include "gui/models/speciesSiteModel.h"
#include "gui/models/speciesTorsionModel.h"
#include "gui/ui_speciesTab.h"

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
    SpeciesTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, QString title, Species *species);
    ~SpeciesTab() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SpeciesTab ui_;
    SpeciesAtomModel atoms_;
    SpeciesIsoModel isos_;
    SpeciesSiteModel sites_;
    // Box information
    QWidget *boxPage_;

    private:
    // Update density label
    void updateDensityLabel();

    public Q_SLOTS:
    // Update controls in tab
    void updateControls() override;
    // Prevent editing within tab
    void preventEditing() override;
    // Allow editing within tab
    void allowEditing() override;

    /*
     * MainTab Reimplementations
     */
    public:
    // Return tab type
    MainTab::TabType type() const override;
    // Raise suitable dialog for entering / checking new tab name
    QString getNewTitle(bool &ok) override;
    // Return whether the title of the tab can be changed
    bool canChangeTitle() const override;
    // Return whether the tab can be closed (after any necessary user querying, etc.)
    bool canClose() const override;

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
     * Widget Functions - Structure
     */
    private Q_SLOTS:
    // Current Box
    void on_DensityUnitsCombo_currentIndexChanged(int index);

    /*
     * Widget Functions - Geometry
     */
    private:
    // Return valid AtomType names for specified model index in the SpeciesAtomTable
    std::vector<std::string> validAtomTypeNames(const QModelIndex &index);

    private Q_SLOTS:
    // Atom table item changed
    void atomTableDataChanged(const QModelIndex &, const QModelIndex &);
    // Respond to changes in the Atom Table
    void updateAtomTableSelection();
    // Respond to selection in the underlying model
    void updateUnderlyingAtomSelection();

    public Q_SLOTS:
    // Update total charges
    void updateTotalCharges();

    /*
     * Widget Functions - Isotopologues
     */
    private Q_SLOTS:
    void isotopologuesSelectionChanged(const QItemSelection &, const QItemSelection &);
    void isotopologuesChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &);
    void on_IsotopologuesTree_customContextMenuRequested(const QPoint &pos);
    void on_IsotopologueAddButton_clicked(bool checked);
    void on_IsotopologueRemoveButton_clicked(bool checked);
    void on_IsotopologueGenerateButton_clicked(bool checked);
    void on_IsotopologueExpandAllButton_clicked(bool checked);
    void on_IsotopologueCollapseAllButton_clicked(bool checked);

    public Q_SLOTS:
    // Update Isotopologues tab
    void updateIsotopologuesTab();

    /*
     * Widget Functions - Intramolecular terms
     */
    void on_IntramolecularTermsButton_clicked(bool checked);

    /*
     * Widget Functions - Sites
     */
    private:
    // Return currently-selected SpeciesSite
    SpeciesSite *currentSite();

    private Q_SLOTS:
    void setCurrentSiteFromViewer();
    void on_SiteRemoveButton_clicked(bool checked);
    void on_SiteTypeCombo_currentIndexChanged(int index);
    void siteSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void on_SiteOriginMassWeightedCheck_clicked(bool checked);
    void on_SiteFragmentDescriptionEdit_editingFinished();
    void on_SiteFragmentDescriptionEdit_returnPressed();
    void buttonGroupToggled(QAbstractButton *button, bool checked);

    public Q_SLOTS:
    // Update sites tab
    void updateSitesTab();
};
