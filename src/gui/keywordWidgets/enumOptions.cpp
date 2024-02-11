// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/enumOptions.h"
#include "gui/helpers/mouseWheelAdjustmentGuard.h"

EnumOptionsKeywordWidget::EnumOptionsKeywordWidget(QWidget *parent, EnumOptionsBaseKeyword *keyword, const CoreData &coreData)
    : QComboBox(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Get the underlying EnumOptionsBase
    const EnumOptionsBase &options = keyword_->baseOptions();

    refreshing_ = true;

    // Populate the combo with the available keywords
    for (auto n = 0; n < options.nOptions(); ++n)
        addItem(QString::fromStdString(std::string(options.keywordByIndex(n))));
    setCurrentIndex(keyword_->enumerationByIndex());

    refreshing_ = false;

    // Turn off editability
    setEditable(false);

    // Connect the currentTextChanged signal to our own slot
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxIndexChanged(int)));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Signals / Slots
 */

// Combo box index changed
void EnumOptionsKeywordWidget::comboBoxIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Use the virtual EnumOptionsBaseKeyword::setEnumerationByIndex() to set the new option and inform the underlying
    // keyword structure that it has been modified
    keyword_->setEnumerationByIndex(index);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Update value displayed in widget
void EnumOptionsKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    // Set the combo box index
    setCurrentIndex(keyword_->enumerationByIndex());

    refreshing_ = false;
}
