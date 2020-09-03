/*
    *** Keyword Widget - BroadeningFunction
    *** src/gui/keywordwidgets/broadeningfunction_funcs.cpp
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

#include "genericitems/listhelper.h"
#include "gui/keywordwidgets/broadeningfunction.h"
#include "gui/keywordwidgets/dropdown.h"
#include <QComboBox>
#include <QHBoxLayout>

BroadeningFunctionKeywordWidget::BroadeningFunctionKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                 const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Add BroadeningFunction types to Combo
    for (int n = 0; n < BroadeningFunction::nFunctionTypes; ++n)
        ui_.FunctionCombo->addItem(
            QString::fromStdString(std::string(BroadeningFunction::functionType((BroadeningFunction::FunctionType)n))));

    // Set deltas on spinboxes
    ui_.Parameter0Spin->setSingleStep(0.01);
    ui_.Parameter1Spin->setSingleStep(0.01);
    ui_.Parameter2Spin->setSingleStep(0.01);
    ui_.Parameter3Spin->setSingleStep(0.01);
    ui_.Parameter4Spin->setSingleStep(0.01);
    ui_.Parameter5Spin->setSingleStep(0.01);

    // Connect signals / slots
    connect(ui_.FunctionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(functionCombo_currentIndexChanged(int)));
    connect(ui_.Parameter0Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
    connect(ui_.Parameter1Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
    connect(ui_.Parameter2Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
    connect(ui_.Parameter3Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
    connect(ui_.Parameter4Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
    connect(ui_.Parameter5Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<BroadeningFunctionKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into BroadeningFunctionKeyword.\n", keyword->name());
    else
    {
        // Set current information
        updateWidgetValues(coreData_);
    }
}

/*
 * Widgets
 */

// Function type combo changed
void BroadeningFunctionKeywordWidget::functionCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    updateKeywordData();

    updateWidgetValues(coreData_);

    emit(keywordValueChanged(keyword_->optionMask()));
}

// Parameter value changed
void BroadeningFunctionKeywordWidget::parameterSpin_valueChanged(double value)
{
    if (refreshing_)
        return;

    updateKeywordData();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void BroadeningFunctionKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void BroadeningFunctionKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Grab the target BroadeningFunction
    auto &broadeningFunction = keyword_->data();

    // Summary text on KeywordDropDown button
    setSummaryText(QString::fromStdString(std::string(BroadeningFunction::functionType(broadeningFunction.function()))));

    // Widgets
    ui_.FunctionCombo->setCurrentIndex(broadeningFunction.function());
    ui_.FunctionDescriptionLabel->setText(
        QString::fromStdString(std::string(BroadeningFunction::functionDescription(broadeningFunction.function()))));

    const auto nParams = BroadeningFunction::nFunctionParameters(broadeningFunction.function());

    std::vector<ExponentialSpin *> spins = {ui_.Parameter0Spin, ui_.Parameter1Spin, ui_.Parameter2Spin,
                                            ui_.Parameter3Spin, ui_.Parameter4Spin, ui_.Parameter5Spin};
    std::vector<QLabel *> labels = {ui_.Parameter0Label, ui_.Parameter1Label, ui_.Parameter2Label,
                                    ui_.Parameter3Label, ui_.Parameter4Label, ui_.Parameter5Label};

    for (auto n = 0; n < MAXBROADENINGFUNCTIONPARAMS; ++n)
    {
        spins[n]->setValue(nParams > n ? broadeningFunction.parameter(0) : 0.0);
        labels[n]->setText(nParams > n ? QString::fromStdString(std::string(broadeningFunction.parameterName(n))) : "N/A");
        spins[n]->setEnabled(nParams > n);
        labels[n]->setEnabled(nParams > n);
    }

    refreshing_ = false;
}

// Update keyword data based on widget values
void BroadeningFunctionKeywordWidget::updateKeywordData()
{
    // Get widget data
    BroadeningFunction broadeningFunction;
    BroadeningFunction::FunctionType func = (BroadeningFunction::FunctionType)ui_.FunctionCombo->currentIndex();
    broadeningFunction.set(func, ui_.Parameter0Spin->value(), ui_.Parameter1Spin->value(), ui_.Parameter2Spin->value(),
                           ui_.Parameter3Spin->value(), ui_.Parameter4Spin->value(), ui_.Parameter5Spin->value());

    keyword_->setData(broadeningFunction);
}
