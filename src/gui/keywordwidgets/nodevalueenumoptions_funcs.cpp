// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/nodevalueenumoptions.h"

NodeValueEnumOptionsKeywordWidget::NodeValueEnumOptionsKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                     const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<NodeValueEnumOptionsBaseKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into NodeValueEnumOptionsBaseKeyword.\n", keyword->name());
    else
    {
        // Get the underlying EnumOptionsBase
        const EnumOptionsBase &options = keyword_->baseOptions();

        // Populate the combo with the available keywords
        for (int n = 0; n < options.nOptions(); ++n)
        {
            ui_.OptionsCombo->addItem(QString::fromStdString(std::string(options.keywordByIndex(n))));
            if (options.index() == n)
                ui_.OptionsCombo->setCurrentIndex(n);
        }

        // Set event filtering on the combo so that we do not blindly accept mouse wheel events (problematic since we
        // will exist in a QScrollArea)
        ui_.OptionsCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.OptionsCombo));

        // Update values
        updateValue();
    }

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
    ui_.ValueValidIndicator->setOK(keyword_->value().isValid());

    emit(keywordValueChanged(keyword_->optionMask()));
}

void NodeValueEnumOptionsKeywordWidget::on_ValueEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setValue(qPrintable(ui_.ValueEdit->text()));
    ui_.ValueValidIndicator->setOK(keyword_->value().isValid());

    emit(keywordValueChanged(keyword_->optionMask()));
}

void NodeValueEnumOptionsKeywordWidget::on_OptionsCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    keyword_->setEnumerationByIndex(index);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void NodeValueEnumOptionsKeywordWidget::updateValue()
{
    refreshing_ = true;

    ui_.ValueEdit->setText(QString::fromStdString(keyword_->value().asString()));
    ui_.ValueValidIndicator->setOK(keyword_->value().isValid());
    ui_.OptionsCombo->setCurrentIndex(keyword_->baseOptions().index());

    refreshing_ = false;
}
