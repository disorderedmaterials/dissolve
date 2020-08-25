/*
    *** Keyword Widget - Bool
    *** src/gui/keywordwidgets/bool_funcs.cpp
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
#include "gui/keywordwidgets/bool.hui"

BoolKeywordWidget::BoolKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QCheckBox(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<BoolKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into BoolKeyword.\n", keyword->name());
    else
    {
        // Set current value
        setChecked(keyword_->asBool());
    }

    // Connect the
    connect(this, SIGNAL(clicked(bool)), this, SLOT(myClicked(bool)));
}

/*
 * Signals / Slots
 */

// Check box state changed
void BoolKeywordWidget::myClicked(bool checked)
{
    if (refreshing_)
        return;

    keyword_->setData(checked);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void BoolKeywordWidget::updateValue()
{
    refreshing_ = true;

    setChecked(keyword_->asBool());

    refreshing_ = false;
}
