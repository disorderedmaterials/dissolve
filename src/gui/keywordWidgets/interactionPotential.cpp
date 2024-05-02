// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/interactionPotential.h"
#include "main/dissolve.h"
#include <QComboBox>

InteractionPotentialKeywordWidget::InteractionPotentialKeywordWidget(QWidget *parent, InteractionPotentialBaseKeyword *keyword,
                                                                     const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    ui_.setupUi(this);

    refreshing_ = true;

    // Populate combo with the available forms
    formModel_.setData(keyword_->formOptions());
    ui_.FormCombo->clear();
    ui_.FormCombo->setModel(&formModel_);

    // Set current information
    updateWidgetValues(coreData_);

    refreshing_ = false;
}

/*
 * Widgets
 */

void InteractionPotentialKeywordWidget::on_ParametersEdit_editingFinished()
{
    if (refreshing_)
        return;

    if (updateKeywordData())
        Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

void InteractionPotentialKeywordWidget::on_ParametersEdit_returnPressed()
{
    if (refreshing_)
        return;

    if (updateKeywordData())
        Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

void InteractionPotentialKeywordWidget::on_FormCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    updateKeywordData();

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Update value displayed in widget
void InteractionPotentialKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void InteractionPotentialKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Update widgets
    ui_.ParametersEdit->setText(QString::fromStdString(std::string(keyword_->parametersAsString())));
    ui_.FormCombo->setCurrentIndex(keyword_->formByIndex());

    refreshing_ = false;
}

// Update keyword data based on widget values, returning if parameters were set successfully
bool InteractionPotentialKeywordWidget::updateKeywordData()
{
    refreshing_ = true;

    keyword_->setFormByIndex(ui_.FormCombo->currentIndex());
    auto parametersOK = keyword_->setParameters(ui_.ParametersEdit->text().toStdString());

    // We always update the displayed text since some "conversion" or trimming may have been performed
    ui_.ParametersEdit->setText(QString::fromStdString(std::string(keyword_->parametersAsString())));

    refreshing_ = false;

    return parametersOK;
}
