// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/nodeValueEnumOptions.h"
#include "gui/helpers/mouseWheelAdjustmentGuard.h"

NodeValueEnumOptionsKeywordWidget::NodeValueEnumOptionsKeywordWidget(QWidget *parent, NodeValueEnumOptionsBaseKeyword *keyword,
                                                                     const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Get the underlying EnumOptionsBase
    const EnumOptionsBase &options = keyword_->baseOptions();

    // Populate the combo with the available keywords
    for (int n = 0; n < options.nOptions(); ++n)
    {
        ui_.OptionsCombo->addItem(QString::fromStdString(std::string(options.keywordByIndex(n))));
        if (keyword_->enumerationIndex() == n)
            ui_.OptionsCombo->setCurrentIndex(n);
    }

    // Set expression text
    ui_.ValueEdit->setText(QString::fromStdString(keyword_->value().asString()));
    checkValueValidity();

    // Set event filtering on the combo so that we do not blindly accept mouse wheel events (problematic since we
    // will exist in a QScrollArea)
    ui_.OptionsCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.OptionsCombo));

    refreshing_ = false;
}

/*
 * Widgets
 */

void NodeValueEnumOptionsKeywordWidget::on_ValueEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setValue(qPrintable(ui_.ValueEdit->text()));
    checkValueValidity();

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

void NodeValueEnumOptionsKeywordWidget::on_ValueEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setValue(qPrintable(ui_.ValueEdit->text()));
    checkValueValidity();

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

void NodeValueEnumOptionsKeywordWidget::on_OptionsCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    keyword_->setEnumeration(index);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Check validity of current value
void NodeValueEnumOptionsKeywordWidget::checkValueValidity() { ui_.ValueValidIndicator->setOK(keyword_->value().isValid()); }

// Update value displayed in widget
void NodeValueEnumOptionsKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    ui_.ValueEdit->setText(QString::fromStdString(keyword_->value().asString()));
    ui_.OptionsCombo->setCurrentIndex(keyword_->enumerationIndex());

    checkValueValidity();

    refreshing_ = false;
}
