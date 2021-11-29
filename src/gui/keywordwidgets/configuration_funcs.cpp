// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/coredata.h"
#include "gui/helpers/comboboxupdater.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/configuration.hui"

ConfigurationKeywordWidget::ConfigurationKeywordWidget(QWidget *parent, ConfigurationKeyword *keyword, const CoreData &coreData)
    : QComboBox(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Set current information
    updateValue();

    // Connect the
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Signals / Slots
 */

// Combo box item changed
void ConfigurationKeywordWidget::comboIndexChanged(int index)
{
    if (refreshing_)
        return;

    Configuration *sp = (index == -1 ? nullptr : VariantPointer<Configuration>(itemData(index, Qt::UserRole)));
    keyword_->data() = sp;
    keyword_->setAsModified();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void ConfigurationKeywordWidget::updateValue()
{
    refreshing_ = true;

    // Update the QComboBox against the global Configuration list
    ComboBoxUpdater<Configuration> comboUpdater(this, coreData_.configurations(), keyword_->data());

    refreshing_ = false;
}
