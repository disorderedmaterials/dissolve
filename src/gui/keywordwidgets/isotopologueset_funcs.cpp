// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/delegates/customcombodelegate.h"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/null.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/isotopologueset.h"
#include "module/module.h"
#include "templates/algorithms.h"

IsotopologueSetKeywordWidget::IsotopologueSetKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set up tree
    ui_.IsotopologueTree->setItemDelegateForColumn(1, new CustomComboDelegate<IsotopologueSetKeywordWidget>(
                                                          this, &IsotopologueSetKeywordWidget::availableIsotopologueNames));
    ui_.IsotopologueTree->setItemDelegateForColumn(2, new ExponentialSpinDelegate(this, 0.0));
    ui_.IsotopologueTree->setModel(&setModel_);

    // Connect signals / slots
    connect(&setModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(&setModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(currentItemChanged()));
    connect(ui_.AddSpeciesButton, SIGNAL(clicked(bool)), this, SLOT(addSpeciesButton_clicked(bool)));
    connect(ui_.AddIsotopologueButton, SIGNAL(clicked(bool)), this, SLOT(addIsotopologueButton_clicked(bool)));
    connect(ui_.RemoveButton, SIGNAL(clicked(bool)), this, SLOT(removeButton_clicked(bool)));
    connect(ui_.IsotopologueTree->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(currentItemChanged()));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<IsotopologueSetKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into IsotopologueSetKeyword.\n", keyword->name());
    else
        setModel_.setSourceData(keyword_->data());

    updateSummaryText();
}

/*
 * Widgets
 */

// Return valid Isotopologue names for specified model index
std::vector<std::string> IsotopologueSetKeywordWidget::availableIsotopologueNames(const QModelIndex &index)
{
    // We are expecting to be given a QModelIndex which locates an item representing an Isotopologue
    // The column of the provided child model index must therefore be 1
    if (!index.parent().isValid() || index.column() != 1)
        throw(
            std::runtime_error("IsotopologueSetKeywordWidget::availableIsotopologueNames() - Not a suitable column (!= 1).\n"));

    // Get the parent
    const auto &topes = keyword_->data().isotopologues()[index.parent().row()];

    // Construct valid names list
    std::vector<std::string> validNames = {"Natural"};
    for (auto &tope : topes.species()->isotopologues())
        validNames.emplace_back(std::string(tope->name()));

    return validNames;
}

void IsotopologueSetKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    keyword_->setAsModified();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

void IsotopologueSetKeywordWidget::addSpeciesButton_clicked(bool checked) { setModel_.addMissingSpecies(coreData_.species()); }

void IsotopologueSetKeywordWidget::addIsotopologueButton_clicked(bool checked)
{
    setModel_.addIsotopologueWeight(ui_.IsotopologueTree->currentIndex());
}

void IsotopologueSetKeywordWidget::removeButton_clicked(bool checked)
{
    setModel_.removeIndex(ui_.IsotopologueTree->currentIndex());
}

void IsotopologueSetKeywordWidget::currentItemChanged()
{
    // Don't use the 'current' parameter to get the selected index as we may be called from a `dataChanged` connection
    auto index = ui_.IsotopologueTree->currentIndex();
    if (!index.isValid())
    {
        ui_.RemoveButton->setEnabled(false);
        ui_.AddIsotopologueButton->setEnabled(false);
    }
    else
    {
        ui_.RemoveButton->setEnabled(true);

        const auto *topes =
            setModel_.data(index.parent().isValid() ? index.parent() : index, Qt::UserRole).value<const Isotopologues *>();
        assert(topes);
        ui_.AddIsotopologueButton->setEnabled(topes->nIsotopologues() < topes->species()->nIsotopologues() + 1);
    }
}

/*
 * Update
 */

// Update value displayed in widget
void IsotopologueSetKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void IsotopologueSetKeywordWidget::updateWidgetValues(const CoreData &coreData) {}

// Update keyword data based on widget values
void IsotopologueSetKeywordWidget::updateKeywordData() {}

// Update summary text
void IsotopologueSetKeywordWidget::updateSummaryText()
{
    std::string text;
    auto nNatural = 0;
    for (const auto &topes : keyword_->data().isotopologues())
        // Check if this is completely "natural" specification
        if (std::count_if(topes.mix().begin(), topes.mix().end(), [&topes](const auto &part) {
                return part.isotopologue() == topes.species()->naturalIsotopologue();
            }) == topes.nIsotopologues())
        {
            text += fmt::format("{}{}[Natural]", text.empty() ? "" : ", ", topes.species()->name());
            ++nNatural;
        }
        else
        {
            if (topes.nIsotopologues() == 1)
                text += fmt::format("{}{}[{}]", text.empty() ? "" : ", ", topes.species()->name(),
                                    topes.mix().front().isotopologue()->name());
            else
                text += fmt::format("{}{}[{}]", text.empty() ? "" : ", ", topes.species()->name(),
                                    joinStrings(topes.mix(), ", ", [](const auto &part) {
                                        return fmt::format("{}={}", part.isotopologue()->name(), part.weight());
                                    }));
        }

    setSummaryText(nNatural == keyword_->data().nSpecies() ? QString("<Default to Natural>") : QString::fromStdString(text));
}
