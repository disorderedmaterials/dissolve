/*
    *** Keyword Widget - String
    *** src/gui/keywordwidgets/stdstring_funcs.cpp
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
#include "gui/keywordwidgets/stdstring.hui"

StringKeywordWidget::StringKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QLineEdit(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<StringKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into StringKeyword.\n", keyword->name());
    else
        setText(QString::fromStdString(keyword_->asString()));

    // Connect the currentTextChanged signal to our own slot
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(myTextChanged(QString)));
}

/*
 * Signals / Slots
 */

// Line edit text changed
void StringKeywordWidget::myTextChanged(const QString &text)
{
    if (refreshing_)
        return;

    keyword_->setData(qPrintable(text));

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void StringKeywordWidget::updateValue()
{
    refreshing_ = true;

    setText(QString::fromStdString(keyword_->asString()));

    refreshing_ = false;
}
