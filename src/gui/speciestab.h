// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/speciesimproper.h"
#include "classes/speciestorsion.h"
#include "gui/maintab.h"
#include "gui/ui_speciestab.h"

// Forward Declarations
class AtomType;
class Isotope;
class Isotopologue;
class Species;

Q_DECLARE_METATYPE(SpeciesAtom *)
Q_DECLARE_METATYPE(SpeciesBond *)
Q_DECLARE_METATYPE(SpeciesAngle *)
Q_DECLARE_METATYPE(SpeciesTorsion *)
Q_DECLARE_METATYPE(SpeciesImproper *)

// Species Tab
class SpeciesTab : public QWidget, public ListItem<SpeciesTab>, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    SpeciesTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
               Species *species);
    ~SpeciesTab();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SpeciesTab ui_;

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

    public:
    // Return displayed Species
    Species *species() const;

    /*
     * Widget Functions - Geometry
     */
    private:
    // Return valid AtomType names for specified model index in the SpeciesAtomTable
    std::vector<std::string> validAtomTypeNames(const QModelIndex &index);
    // SpeciesAtomTable row update function
    void updateAtomTableRow(int row, SpeciesAtom *speciesAtom, bool createItems);
    // SpeciesBondTable row update function
    void updateBondTableRow(int row, SpeciesBond *speciesBond, bool createItems);
    // SpeciesAngleTable row update function
    void updateAngleTableRow(int row, SpeciesAngle *speciesAngle, bool createItems);
    // SpeciesTorsionTable row update function
    void updateTorsionTableRow(int row, SpeciesTorsion *speciesTorsion, bool createItems);
    // SpeciesImproperTable row update function
    void updateImproperTableRow(int row, SpeciesImproper *speciesImproper, bool createItems);

    private slots:
    // Update atom table selection
    void updateAtomTableSelection();

    private slots:
    void on_AtomTable_itemChanged(QTableWidgetItem *w);
    void on_AtomTable_itemSelectionChanged();
    void on_BondTable_itemChanged(QTableWidgetItem *w);
    void on_AngleTable_itemChanged(QTableWidgetItem *w);
    void on_TorsionTable_itemChanged(QTableWidgetItem *w);
    void on_ImproperTable_itemChanged(QTableWidgetItem *w);

    public slots:
    // Update Geometry tab
    void updateGeometryTab();

    /*
     * Widget Functions - Isotopologues
     */
    private:
    // IsotopologuesTree top-level update function
    void updateIsotopologuesTreeTopLevelItem(QTreeWidget *treeWidget, int topLevelItemIndex, const Isotopologue *data,
                                             bool createItem);
    // IsotopologuesTree item update function
    void updateIsotopologuesTreeChildItem(QTreeWidgetItem *parentItem, int childIndex, std::shared_ptr<AtomType> item,
                                          Isotope *data, bool createItem);
    // Return currently-selected Isotopologue
    Isotopologue *currentIsotopologue();

    private slots:
    void on_IsotopologueAddButton_clicked(bool checked);
    void on_IsotopologueRemoveButton_clicked(bool checked);
    void on_IsotopologueGenerateButton_clicked(bool checked);
    void on_IsotopologueExpandAllButton_clicked(bool checked);
    void on_IsotopologueCollapseAllButton_clicked(bool checked);
    void on_IsotopologuesTree_itemChanged(QTreeWidgetItem *item, int column);

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
    void on_SiteList_currentItemChanged(QListWidgetItem *currentItem, QListWidgetItem *previousItem);
    void on_SiteList_itemChanged(QListWidgetItem *item);
    void on_SiteOriginMassWeightedCheck_clicked(bool checked);

    public slots:
    // Update sites tab
    void updateSitesTab();
};
