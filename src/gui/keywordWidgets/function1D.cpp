// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/function1D.h"
#include "gui/keywordWidgets/dropDown.h"
#include <QComboBox>
#include <QHBoxLayout>

#define MaxParams 6

Function1DKeywordWidget::Function1DKeywordWidget(QWidget *parent, Function1DKeyword *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    refreshing_ = true;

    // Grab widget pointers for convenience
    spins_ = {ui_.Parameter0Spin, ui_.Parameter1Spin, ui_.Parameter2Spin,
              ui_.Parameter3Spin, ui_.Parameter4Spin, ui_.Parameter5Spin};
    labels_ = {ui_.Parameter0Label, ui_.Parameter1Label, ui_.Parameter2Label,
               ui_.Parameter3Label, ui_.Parameter4Label, ui_.Parameter5Label};

    // Set deltas on spinboxes
    for (auto *spin : spins_)
        spin->setSingleStep(0.01);

    // Grab the target Function1D
    auto &function = keyword_->data();

    // Get relevant function types to show in combo
    auto availableFunctions = Functions1D::matchingFunction1D(keyword_->functionProperties());
    if (availableFunctions.empty())
    {
        ui_.FunctionCombo->addItem("<None Available>");
        ui_.FunctionCombo->setEnabled(false);
    }
    else
    {
        auto i = 0;
        for (auto func : availableFunctions)
        {
            ui_.FunctionCombo->addItem(QString::fromStdString(Functions1D::forms().keyword(func)), QVariant::fromValue(func));
            if (func == function.form())
                ui_.FunctionCombo->setCurrentIndex(i);
            ++i;
        }
    }

    // Update all widgets
    updateWidgetsFromData();

    // Update summary text
    updateSummaryText();

    refreshing_ = false;

    // Connect signals / slots
    connect(ui_.FunctionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(functionCombo_currentIndexChanged(int)));
    for (auto *spin : spins_)
        connect(spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
}

/*
 * Widgets
 */

// Function type combo changed
void Function1DKeywordWidget::functionCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    updateKeywordData();

    updateSummaryText();

    updateWidgetsFromData();

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

// Parameter value changed
void Function1DKeywordWidget::parameterSpin_valueChanged(double value)
{
    if (refreshing_)
        return;

    updateKeywordData();

    updateSummaryText();

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Update widgets, including visibility of spins and labels
void Function1DKeywordWidget::updateWidgetsFromData()
{
    refreshing_ = true;

    // Grab the target Function1D
    auto &function = keyword_->data();

    ui_.FunctionCombo->setCurrentIndex(ui_.FunctionCombo->findData(QVariant::fromValue(function.form())));

    const auto nParams = function.nParameters();
    for (auto n = 0; n < MaxParams; ++n)
    {
        spins_[n]->setValue(nParams > n ? function.parameters()[n] : 0.0);
        labels_[n]->setText(nParams > n ? QString::fromStdString(function.parameterName(n)) : "N/A");
        spins_[n]->setVisible(nParams > n);
        labels_[n]->setVisible(nParams > n);
    }

    refreshing_ = false;
}

// Update value displayed in widget
void Function1DKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    updateWidgetsFromData();
}

// Update keyword data based on widget values
void Function1DKeywordWidget::updateKeywordData()
{
    // Get current data from widgets
    auto func = ui_.FunctionCombo->currentData().value<Functions1D::Form>();
    std::vector<double> newParams;
    for (auto n = 0; n < Functions1D::functionDefinition1D(func).nParameters(); ++n)
        newParams.push_back(spins_[n]->value());

    // Set new data
    keyword_->setData({func, newParams});
}

// Update summary text
void Function1DKeywordWidget::updateSummaryText()
{
    // Grab the target Function1D
    auto &function = keyword_->data();

    // Summary text on KeywordDropDown button
    setSummaryText(QString::fromStdString(
        function.nParameters() == 0
            ? Functions1D::forms().keyword(function.form())
            : fmt::format("{} ({})", Functions1D::forms().keyword(function.form()), function.parameterSummary())));
}
