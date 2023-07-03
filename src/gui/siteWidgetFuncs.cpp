// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/empiricalformula.h"
#include "classes/species.h"
#include "gui/sitewidget.h"
#include "gui/widgets/elementselector.hui"
#include "main/dissolve.h"
#include <QButtonGroup>

SiteWidget::SiteWidget(QWidget *parent) : QWidget(parent)
{
    // Set up our UI
    ui_.setupUi(this);

    // Connect signals / slots
    connect(ui_.SiteView, SIGNAL(dataModified()), this, SLOT(notifyDataModified()));
    connect(ui_.SiteView, SIGNAL(styleModified()), this, SLOT(notifyStyleModified()));
    connect(ui_.SiteView, SIGNAL(atomsChanged()), this, SLOT(updateStatusBar()));
    connect(ui_.SiteView, SIGNAL(atomsChanged()), this, SLOT(updateToolbar()));
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
        case (SiteViewer::InteractionMode::Select):
        case (SiteViewer::InteractionMode::SelectArea):
            ui_.InteractionViewButton->setChecked(true);
            break;
        default:
            break;
    }

    // Set checkable buttons
    ui_.ViewSpheresButton->setChecked(siteViewer()->speciesRenderableDrawStyle() != RenderableSpecies::LinesStyle);

    // Enable site-definition buttons
    auto currentSelection = (siteViewer()->species() ? !siteViewer()->species()->selectedAtoms().empty() : false);
    auto siteType = siteViewer()->speciesSite() ? siteViewer()->speciesSite()->type() : SpeciesSite::SiteType::Static;
    ui_.SiteCreateButton->setEnabled(currentSelection);
    ui_.SiteSetOriginButton->setEnabled(currentSelection && siteViewer()->speciesSite() &&
                                        siteType == SpeciesSite::SiteType::Static);
    ui_.SiteSetXAxisButton->setEnabled(currentSelection && siteViewer()->speciesSite() &&
                                       siteType == SpeciesSite::SiteType::Static);
    ui_.SiteSetYAxisButton->setEnabled(currentSelection && siteViewer()->speciesSite() &&
                                       siteType == SpeciesSite::SiteType::Static);
    ui_.SiteSetElementsButton->setEnabled(currentSelection && siteViewer()->speciesSite() &&
                                          siteType == SpeciesSite::SiteType::Dynamic);
    ui_.SiteSetAtomTypesButton->setEnabled(currentSelection && siteViewer()->speciesSite() &&
                                           siteType == SpeciesSite::SiteType::Dynamic);
}

// Update status bar
void SiteWidget::updateStatusBar()
{
    // Get displayed Species
    const auto *sp = siteViewer()->species();

    // Set interaction mode text
    ui_.ModeLabel->setText(siteViewer()->interactionModeText());

    // Set / update empirical formula for the Species and its current atom selection
    if (sp)
    {
        auto selection = sp->selectedAtoms();
        ui_.FormulaLabel->setText(QString::fromStdString(EmpiricalFormula::formula(
            sp->atoms(), [](const auto &i) { return i.Z(); }, true)));
        ui_.SelectionLabel->setText(!selection.empty() ? QString::fromStdString(EmpiricalFormula::formula(
                                                             selection, [](const auto &i) { return i->Z(); }, true))
                                                       : "--");
    }
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
        siteViewer()->setInteractionMode(SiteViewer::InteractionMode::Select);
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
    if ((!sp) || sp->selectedAtoms().empty())
        return;

    // Create the new site, using the empirical formula of the selection as the base name
    auto *site = sp->addSite(EmpiricalFormula::formula(sp->selectedAtoms(), [](const auto &i) { return i->Z(); }));
    site->setStaticOriginAtoms(sp->selectedAtoms());

    // Update the siteViewer
    siteViewer()->setSite(site);
    siteViewer()->postRedisplay();

    emit(siteCreatedAndShown());
}

void SiteWidget::on_SiteSetOriginButton_clicked(bool checked)
{
    // Sanity check for valid SpeciesSite and Species
    auto *sp = siteViewer()->species();
    auto *site = siteViewer()->speciesSite();
    if (!site)
        return;

    // Get current atom selection from Species
    site->setStaticOriginAtoms(sp->selectedAtoms());

    siteViewer()->postRedisplay();

    emit(dataModified());
}

void SiteWidget::on_SiteSetXAxisButton_clicked(bool checked)
{
    // Sanity check for valid SpeciesSite and Species
    auto *sp = siteViewer()->species();
    auto *site = siteViewer()->speciesSite();
    if (!site)
        return;

    // Get current atom selection from Species
    site->setStaticXAxisAtoms(sp->selectedAtoms());

    siteViewer()->postRedisplay();

    emit(dataModified());
}

void SiteWidget::on_SiteSetYAxisButton_clicked(bool checked)
{
    // Sanity check for valid SpeciesSite and Species
    auto *sp = siteViewer()->species();
    auto *site = siteViewer()->speciesSite();
    if (!site)
        return;

    // Get current atom selection from Species
    site->setStaticYAxisAtoms(sp->selectedAtoms());

    siteViewer()->postRedisplay();

    emit(dataModified());
}

void SiteWidget::on_SiteSetElementsButton_clicked(bool checked)
{
    // Sanity check for valid SpeciesSite and Species
    auto *sp = siteViewer()->species();
    auto *site = siteViewer()->speciesSite();
    if (!site)
        return;

    // Get current atom selection from Species
    for (auto &i : sp->selectedAtoms())
        site->addDynamicElement(i->Z());

    siteViewer()->postRedisplay();

    emit(dataModified());
}

void SiteWidget::on_SiteSetAtomTypesButton_clicked(bool checked)
{
    // Sanity check for valid SpeciesSite and Species
    auto *sp = siteViewer()->species();
    auto *site = siteViewer()->speciesSite();
    if (!site)
        return;

    // Get current atom selection from Species
    for (auto &i : sp->selectedAtoms())
        if (i->atomType())
            site->addDynamicAtomType(i->atomType());

    siteViewer()->postRedisplay();

    emit(dataModified());
}
