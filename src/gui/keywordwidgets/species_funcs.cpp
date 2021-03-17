// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/helpers/comboboxupdater.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/species.hui"

SpeciesKeywordWidget::SpeciesKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QComboBox(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<SpeciesKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into SpeciesKeyword.\n", keyword->name());
    else
    {
        // Set current information
        updateValue();
    }

    // Connect the
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myIndexChanged(int)));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Signals / Slots
 */

// Combo box item changed
void SpeciesKeywordWidget::myIndexChanged(int index)
{
    if (refreshing_)
        return;

    Species *sp = (index == -1 ? nullptr : VariantPointer<Species>(itemData(index, Qt::UserRole)));
    keyword_->setData(sp);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesKeywordWidget::updateValue()
{
    refreshing_ = true;

    // Update the QComboBox against the global Species list
    ComboBoxUpdater<Species> comboUpdater(this, coreData_.species(), keyword_->data());

    refreshing_ = false;
}
