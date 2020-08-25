/*
    *** Species Widget - Functions
    *** src/gui/sitewidget_funcs.cpp
    Copyright T. Youngs 2013-2020

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

#include "classes/coredata.h"
#include "classes/empiricalformula.h"
#include "classes/species.h"
#include "gui/sitewidget.h"
#include "gui/widgets/elementselector.hui"
#include "main/dissolve.h"
#include "procedure/nodes/addspecies.h"
#include "procedure/nodes/box.h"
#include <QButtonGroup>

SiteWidget::SiteWidget(QWidget *parent) : QWidget(parent)
{
    // Set up our UI
    ui_.setupUi(this);

    // 	// Create a button group for the interaction modes
    // 	QButtonGroup* group = new QButtonGroup;
    // 	group->addButton(ui_.InteractionViewButton);
    // 	group->addButton(ui_.InteractionDrawButton);

    // Connect signals / slots
    connect(ui_.SiteView, SIGNAL(dataModified()), this, SLOT(notifyDataModified()));
    connect(ui_.SiteView, SIGNAL(styleModified()), this, SLOT(notifyStyleModified()));
    connect(ui_.SiteView, SIGNAL(atomSelectionChanged()), this, SLOT(updateStatusBar()));
    connect(ui_.SiteView, SIGNAL(atomSelectionChanged()), this, SLOT(updateToolbar()));
    connect(ui_.SiteView, SIGNAL(interactionModeChanged()), this, SLOT(updateStatusBar()));

    // Make sure our controls are consistent with the underlying viewer / data
    updateToolbar();
    updateStatusBar();
}

SiteWidget::~SiteWidget() {}

// Set main CoreData pointer
void SiteWidget::setCoreData(CoreData *coreData) { coreData_ = coreData; }

/*
 * UI
 */

// Notify that the style of displayed data in the underlying viewer has changed
void SiteWidget::notifyStyleModified() { emit(styleModified()); }

// Notify that the displayed data in the underlying viewer has changed
void SiteWidget::notifyDataModified() { emit(dataModified()); }

// Post redisplay in the underlying view
void SiteWidget::postRedisplay() { ui_.SiteView->postRedisplay(); }

// Update toolbar to reflect current viewer state
void SiteWidget::updateToolbar()
{
    // Set current interaction mode
    switch (siteViewer()->interactionMode())
    {
        case (SiteViewer::DefaultInteraction):
            ui_.InteractionViewButton->setChecked(true);
            break;
    }

    // Set checkable buttons
    ui_.ViewSpheresButton->setChecked(siteViewer()->speciesRenderableDrawStyle() != RenderableSpecies::LinesStyle);

    // Enable site-definition buttons
    auto currentSelection = (siteViewer()->species() ? siteViewer()->species()->nSelectedAtoms() != 0 : false);
    ui_.SiteCreateButton->setEnabled(currentSelection);
    ui_.SiteSetOriginButton->setEnabled(currentSelection && siteViewer()->speciesSite());
    ui_.SiteSetXAxisButton->setEnabled(currentSelection && siteViewer()->speciesSite());
    ui_.SiteSetYAxisButton->setEnabled(currentSelection && siteViewer()->speciesSite());
}

// Update status bar
void SiteWidget::updateStatusBar()
{
    // Get displayed Species
    const Species *sp = siteViewer()->species();

    // Set interaction mode text
    ui_.ModeLabel->setText(siteViewer()->interactionModeText());

    // Set / update empirical formula for the Species and its current atom selection
    ui_.FormulaLabel->setText(sp ? QString::fromStdString(EmpiricalFormula::formula(sp, true)) : "--");
    ui_.SelectionLabel->setText(
        sp && (sp->nSelectedAtoms() > 0) ? QString::fromStdString(EmpiricalFormula::formula(sp->selectedAtoms(), true)) : "--");
}

/*
 * Site Viewer
 */

// Set parent Species for site
void SiteWidget::setSpecies(Species *sp)
{
    ui_.SiteView->setSpecies(sp);

    updateToolbar();
}

// Set target site, updating widget as necessary
void SiteWidget::setSite(SpeciesSite *site)
{
    ui_.SiteView->setSite(site);

    updateToolbar();
    updateStatusBar();
}

// Return contained SiteViewer
SiteViewer *SiteWidget::siteViewer() { return ui_.SiteView; }

/*
 * Toolbar
 */

void SiteWidget::on_InteractionViewButton_clicked(bool checked)
{
    if (checked)
        siteViewer()->setInteractionMode(SiteViewer::DefaultInteraction);
}

void SiteWidget::on_ViewResetButton_clicked(bool checked)
{
    siteViewer()->view().showAllData();
    siteViewer()->view().resetViewMatrix();

    siteViewer()->postRedisplay();
}

void SiteWidget::on_ViewSpheresButton_clicked(bool checked)
{
    siteViewer()->setSpeciesRenderableDrawStyle(checked ? RenderableSpecies::SpheresStyle : RenderableSpecies::LinesStyle);

    siteViewer()->notifyDataModified();

    siteViewer()->postRedisplay();
}

void SiteWidget::on_ViewCopyToClipboardButton_clicked(bool checked) { siteViewer()->copyViewToClipboard(checked); }

void SiteWidget::on_SiteCreateButton_clicked(bool checked)
{
    // Sanity check for valid Species and selection
    auto *sp = siteViewer()->species();
    if ((!sp) || (sp->nSelectedAtoms() == 0))
        return;

    // Create the new site, using the empirical formula of the selection as the base name
    auto *site = sp->addSite(EmpiricalFormula::formula(sp->selectedAtoms()));
    site->setOriginAtoms(sp->selectedAtoms());

    // Update the siteViewer
    siteViewer()->setSite(site);
    siteViewer()->postRedisplay();

    emit(siteCreatedAndShown());
}

void SiteWidget::on_SiteSetOriginButton_clicked(bool checked)
{
    // Sanity check for valid SpeciesSite and Species
    Species *sp = siteViewer()->species();
    SpeciesSite *site = siteViewer()->speciesSite();
    if (!site)
        return;

    // Get current atom selection from Species
    site->setOriginAtoms(sp->selectedAtoms());

    siteViewer()->postRedisplay();

    emit(dataModified());
}

void SiteWidget::on_SiteSetXAxisButton_clicked(bool checked)
{
    // Sanity check for valid SpeciesSite and Species
    Species *sp = siteViewer()->species();
    SpeciesSite *site = siteViewer()->speciesSite();
    if (!site)
        return;

    // Get current atom selection from Species
    site->setXAxisAtoms(sp->selectedAtoms());

    siteViewer()->postRedisplay();

    emit(dataModified());
}

void SiteWidget::on_SiteSetYAxisButton_clicked(bool checked)
{
    // Sanity check for valid SpeciesSite and Species
    Species *sp = siteViewer()->species();
    SpeciesSite *site = siteViewer()->speciesSite();
    if (!site)
        return;

    // Get current atom selection from Species
    site->setYAxisAtoms(sp->selectedAtoms());

    siteViewer()->postRedisplay();

    emit(dataModified());
}
