// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/speciestab.h"
#include "templates/algorithms.h"
#include <string>

/*
 * Private Functions
 */

// Return currently-selected SpeciesSite
SpeciesSite *SpeciesTab::currentSite()
{
    return sites_.data(ui_.SiteList->selectionModel()->currentIndex(), Qt::UserRole).value<SpeciesSite *>();
}

/*
 * Private Slots
 */

void SpeciesTab::setCurrentSiteFromViewer()
{
    // Get the currently-displayed site in the SiteViewer - if different from ours, change our controls to reflect it
    auto *displayedSite = ui_.SiteViewerWidget->siteViewer()->speciesSite();
    if (!displayedSite)
        return;

    // Sanity check that the displayed site actually exists in our species, and get its index in the sites list
    auto it = std::find_if(species_->sites().begin(), species_->sites().end(),
                           [displayedSite](const auto &site) { return site.get() == displayedSite; });
    auto index = it - species_->sites().begin();
    if (it == species_->sites().end())
        return;

    // Force a refresh of the sites model
    sites_.setData(species_->sites());
    ui_.SiteList->setCurrentIndex(sites_.index(index, 0));

    dissolveWindow_->setModified();

    dissolveWindow_->fullUpdate();
}

void SpeciesTab::on_SiteAddButton_clicked(bool checked)
{
    species_->addSite("NewSite");

    sites_.setData(species_->sites());

    dissolveWindow_->setModified(DissolveSignals::SpeciesSiteMutated);

    dissolveWindow_->fullUpdate();
}

void SpeciesTab::on_SiteRemoveButton_clicked(bool checked)
{
    // Get the currently-selected site
    auto *site = currentSite();
    if (!site)
        return;

    // Remove references to the site, and invalidate our site renderable
    dissolveWindow_->dissolve().removeReferencesTo(site);
    ui_.SiteViewerWidget->setSite(nullptr);

    // Remove the site proper
    dissolve_.removeReferencesTo(site);
    species_->removeSite(site);
    sites_.setData(species_->sites());

    dissolveWindow_->setModified(DissolveSignals::SpeciesSiteMutated);

    dissolveWindow_->fullUpdate();
}

void SpeciesTab::on_SiteTypeCombo_currentIndexChanged(int index)
{
    auto *site = currentSite();
    if (refreshLock_.isLocked() || (!site))
        return;

    switch (index)
    {
        case (0):
            site->setType(SpeciesSite::SiteType::Static);
            break;
        case (1):
            site->setType(SpeciesSite::SiteType::Dynamic);
            break;
        case (2):
            site->setType(SpeciesSite::SiteType::Fragment);
            break;
        default:
            Messenger::error("Type combo index '{}' not handled.\n", index);
    }

    updateSitesTab();

    ui_.SiteViewerWidget->postRedisplay();

    dissolveWindow_->setModified();
}

void SpeciesTab::siteSelectionChanged(const QItemSelection &current, const QItemSelection &previous) { updateSitesTab(); }

void SpeciesTab::on_SiteOriginMassWeightedCheck_clicked(bool checked)
{
    auto *site = currentSite();
    if (refreshLock_.isLocked() || (!site))
        return;

    site->setOriginMassWeighted(checked);

    ui_.SiteViewerWidget->postRedisplay();

    dissolveWindow_->setModified();
}

void SpeciesTab::on_SiteFragmentDescriptionEdit_editingFinished()
{
    auto *site = currentSite();
    if (refreshLock_.isLocked() || (!site))
        return;

    site->setFragmentDefinitionString(std::string_view(ui_.SiteFragmentDescriptionEdit->text().toStdString()));
    ui_.DescriptionValidIndicator->setOK(site->fragment().isValid());

    ui_.SiteViewerWidget->postRedisplay();

    dissolveWindow_->setModified();
}

void SpeciesTab::on_SiteFragmentDescriptionEdit_returnPressed()
{
    auto *site = currentSite();
    if (refreshLock_.isLocked() || (!site))
        return;
    site->setFragmentDefinitionString(std::string_view(ui_.SiteFragmentDescriptionEdit->text().toStdString()));
    ui_.DescriptionValidIndicator->setOK(site->fragment().isValid());

    ui_.SiteViewerWidget->postRedisplay();

    dissolveWindow_->setModified();
}

/*
 * Public Functions
 */

std::string siteName(const SpeciesAtom &i)
{
    return i.atomType() ? fmt::format("{} [{}]", i.index() + 1, i.atomType()->name()) : fmt::format("{}", i.index() + 1);
}

// Update sites tab
void SpeciesTab::updateSitesTab()
{
    Locker refreshLocker(refreshLock_);

    // Check for current site
    auto *site = currentSite();
    if (!site && sites_.rowCount() > 0)
    {
        ui_.SiteList->setCurrentIndex(sites_.index(0, 0));
        site = currentSite();
    }

    ui_.SiteRemoveButton->setEnabled(site != nullptr);
    ui_.SiteTypeCombo->setEnabled(site != nullptr);
    ui_.SiteDefinitionStack->setEnabled(site != nullptr);
    if (!site)
    {
        ui_.SiteOriginAtomsEdit->clear();
        ui_.SiteXAxisAtomsEdit->clear();
        ui_.SiteYAxisAtomsEdit->clear();
        ui_.SiteElementsEdit->clear();
        ui_.SiteAtomTypesEdit->clear();
        return;
    }

    if (site->type() == SpeciesSite::SiteType::Static)
    {
        ui_.SiteTypeCombo->setCurrentIndex(0);
        ui_.SiteDefinitionStack->setCurrentIndex(0);

        // Set origin atom indices
        ui_.SiteOriginAtomsEdit->setText(
            QString::fromStdString(joinStrings(site->originAtoms(), " ", [](const auto &i) { return siteName(*i); })));
        ui_.SiteOriginMassWeightedCheck->setCheckState(site->originMassWeighted() ? Qt::Checked : Qt::Unchecked);

        // Set x axis atom indices
        ui_.SiteXAxisAtomsEdit->setText(
            QString::fromStdString(joinStrings(site->xAxisAtoms(), " ", [](const auto &i) { return siteName(*i); })));

        // Set y axis atom indices
        ui_.SiteYAxisAtomsEdit->setText(
            QString::fromStdString(joinStrings(site->yAxisAtoms(), " ", [](const auto &i) { return siteName(*i); })));
    }
    else if (site->type() == SpeciesSite::SiteType::Dynamic)
    {
        ui_.SiteTypeCombo->setCurrentIndex(1);
        ui_.SiteDefinitionStack->setCurrentIndex(1);

        // Set elements
        ui_.SiteElementsEdit->setText(
            QString::fromStdString(joinStrings(site->elements(), " ", [](const auto &el) { return Elements::symbol(el); })));

        // Set atom types
        ui_.SiteAtomTypesEdit->setText(
            QString::fromStdString(joinStrings(site->atomTypes(), " ", [](const auto &at) { return at->name(); })));
    }
    else if (site->type() == SpeciesSite::SiteType::Fragment)
    {
        ui_.SiteTypeCombo->setCurrentIndex(2);
        ui_.SiteDefinitionStack->setCurrentIndex(2);
        ui_.SiteFragmentDescriptionEdit->setText(QString::fromStdString(std::string(site->fragment().definitionString())));
        ui_.DescriptionValidIndicator->setOK(site->fragment().isValid());
    }

    // If the current site has changed, also regenerate the SpeciesSite renderable
    if (ui_.SiteViewerWidget->siteViewer()->speciesSite() != site)
        ui_.SiteViewerWidget->setSite(site);
}
