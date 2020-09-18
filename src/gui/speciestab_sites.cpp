/*
    *** SpeciesTab Functions - Sites
    *** src/gui/speciestab_sites.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    if (!species_->sites().contains(displayedSite))
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
    Array<int> originAtoms = site->originAtomIndices();
    QString originText = originAtoms.nItems() == 0 ? QString() : QString::number(originAtoms.constAt(0) + 1);
    for (int n = 1; n < originAtoms.nItems(); ++n)
        originText += QString(" %1").arg(originAtoms.constAt(n) + 1);
    ui_.SiteOriginAtomsEdit->setText(originText);
    ui_.SiteOriginMassWeightedCheck->setCheckState(site->originMassWeighted() ? Qt::Checked : Qt::Unchecked);

    // Set x axis atom indices
    Array<int> xAxisAtoms = site->xAxisAtomIndices();
    QString xAxisText = xAxisAtoms.nItems() == 0 ? QString() : QString::number(xAxisAtoms.constAt(0) + 1);
    for (int n = 1; n < xAxisAtoms.nItems(); ++n)
        xAxisText += QString(" %1").arg(xAxisAtoms.constAt(n) + 1);
    ui_.SiteXAxisAtomsEdit->setText(xAxisText);

    // Set y axis atom indices
    Array<int> yAxisAtoms = site->yAxisAtomIndices();
    QString yAxisText = yAxisAtoms.nItems() == 0 ? QString() : QString::number(yAxisAtoms.constAt(0) + 1);
    for (int n = 1; n < yAxisAtoms.nItems(); ++n)
        yAxisText += QString(" %1").arg(yAxisAtoms.constAt(n) + 1);
    ui_.SiteYAxisAtomsEdit->setText(yAxisText);

    // If the current site has changed, also regenerate the SpeciesSite renderable
    if (current != site)
        ui_.SiteViewerWidget->setSite(site);
}
