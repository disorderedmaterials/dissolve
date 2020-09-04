/*
    *** Keyword Widget - Species
    *** src/gui/keywordwidgets/species_funcs.cpp
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
    ComboBoxUpdater<Species> comboUpdater(this, coreData_.constSpecies(), keyword_->data());

    refreshing_ = false;
}
