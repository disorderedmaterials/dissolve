// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/speciestab.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"

/*
 * Private Functions
 */

// Return currently-selected SpeciesSite
SpeciesSite *SpeciesTab::currentSite()
{
    // Get current item from tree, and check the parent item
    QListWidgetItem *item = ui_.SiteList->currentItem();
    if (!item)
        return nullptr;
    return VariantPointer<SpeciesSite>(item->data(Qt::UserRole));
}

/*
 * Private Slots
 */

void SpeciesTab::setCurrentSiteFromViewer()
{
    Locker refreshLocker(refreshLock_);

    // Get the currently-displayed site in the SiteViewer - if different from ours, change our controls to reflect it
    SpeciesSite *displayedSite = ui_.SiteViewerWidget->siteViewer()->speciesSite();
    if (!displayedSite)
        return;

    // Sanity check that the displayed site actually exists in our species
    if (std::find_if(species_->sites().begin(), species_->sites().end(),
                     [displayedSite](const auto &p) { return &p == displayedSite; }) == species_->sites().end())
        return;

    // Update the site list
    ListWidgetUpdater<SpeciesTab, SpeciesSite> siteUpdater(
        ui_.SiteList, species_->sites(), Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable, displayedSite);

    refreshLocker.unlock();

    // Now update the tab
    updateSitesTab();

    dissolveWindow_->setModified();
}

void SpeciesTab::on_SiteAddButton_clicked(bool checked)
{
    species_->addSite("NewSite");

    updateSitesTab();
}

void SpeciesTab::on_SiteRemoveButton_clicked(bool checked)
{
    // Get the currently-selected site
    SpeciesSite *site = currentSite();
    if (!site)
        return;

    // Remove references to the site, and invalidate our site renderable
    dissolveWindow_->dissolve().removeReferencesTo(site);
    ui_.SiteViewerWidget->setSite(nullptr);

    // Remove the site proper, and update the sites tab
    species_->removeSite(site);
    updateSitesTab();

    dissolveWindow_->setModified();

    dissolveWindow_->fullUpdate();
}

void SpeciesTab::on_SiteList_currentItemChanged(QListWidgetItem *currentItem, QListWidgetItem *previousItem)
{
    if (refreshLock_.isLocked())
        return;

    ui_.SiteViewerWidget->setSite(currentSite());

    updateSitesTab();
}

void SpeciesTab::on_SiteList_itemChanged(QListWidgetItem *item)
{
    if (refreshLock_.isLocked())
        return;

    // Get the site pointer from the item
    SpeciesSite *site = VariantPointer<SpeciesSite>(item->data(Qt::UserRole));
    if (!site)
        return;

    // Set unique site name
    site->setName(species_->uniqueSiteName(qPrintable(item->text()), site));

    dissolveWindow_->setModified();
}

void SpeciesTab::on_SiteOriginMassWeightedCheck_clicked(bool checked)
{
    SpeciesSite *site = currentSite();
    if (refreshLock_.isLocked() || (!site))
        return;

    site->setOriginMassWeighted(checked);

    ui_.SiteViewerWidget->postRedisplay();

    dissolveWindow_->setModified();
}

/*
 * Public Functions
 */

// Update sites tab
void SpeciesTab::updateSitesTab()
{
    Locker refreshLocker(refreshLock_);

    // Check for valid species
    if (!species_)
    {
        ui_.SiteList->clear();
        return;
    }

    // Grab and store current site for reference
    SpeciesSite *current = currentSite();

    // Update the site list
    ListWidgetUpdater<SpeciesTab, SpeciesSite> siteUpdater(ui_.SiteList, species_->sites(),
                                                           Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    if ((current == nullptr) && (species_->nSites() != 0))
        ui_.SiteList->setCurrentRow(0);

    // Check for current site
    SpeciesSite *site = currentSite();
    ui_.SiteRemoveButton->setEnabled(site != nullptr);
    ui_.SiteOriginGroup->setEnabled(site != nullptr);
    ui_.SiteXAxisGroup->setEnabled(site != nullptr);
    ui_.SiteYAxisGroup->setEnabled(site != nullptr);
    if (!site)
    {
        ui_.SiteOriginAtomsEdit->clear();
        ui_.SiteXAxisAtomsEdit->clear();
        ui_.SiteYAxisAtomsEdit->clear();
        return;
    }

    // Set origin atom indices
    ui_.SiteOriginAtomsEdit->setText(
        std::accumulate(site->originAtomIndices().begin(), site->originAtomIndices().end(), QString(),
                        [](auto &val, auto idx) { return val + QString("%1%2").arg(val.isEmpty() ? "" : " ").arg(idx + 1); }));
    ui_.SiteOriginMassWeightedCheck->setCheckState(site->originMassWeighted() ? Qt::Checked : Qt::Unchecked);

    // Set x axis atom indices
    ui_.SiteXAxisAtomsEdit->setText(
        std::accumulate(site->xAxisAtomIndices().begin(), site->xAxisAtomIndices().end(), QString(),
                        [](auto &val, auto idx) { return val + QString("%1%2").arg(val.isEmpty() ? "" : " ").arg(idx + 1); }));

    // Set y axis atom indices
    ui_.SiteYAxisAtomsEdit->setText(
        std::accumulate(site->yAxisAtomIndices().begin(), site->yAxisAtomIndices().end(), QString(),
                        [](auto &val, auto idx) { return val + QString("%1%2").arg(val.isEmpty() ? "" : " ").arg(idx + 1); }));

    // If the current site has changed, also regenerate the SpeciesSite renderable
    if (current != site)
        ui_.SiteViewerWidget->setSite(site);
}
