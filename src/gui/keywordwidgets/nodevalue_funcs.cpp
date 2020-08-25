/*
    *** Keyword Widget - NodeValue
    *** src/gui/keywordwidgets/nodevalue_funcs.cpp
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

#include "gui/keywordwidgets/nodevalue.h"

NodeValueKeywordWidget::NodeValueKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<NodeValueKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into NodeValueKeyword.\n", keyword->name());
    else
    {
        // Set expression text
        ui_.ValueEdit->setText(QString::fromStdString(keyword_->data().asString()));
        checkValueValidity();
    }

    refreshing_ = false;
}

/*
 * Widgets
 */

void NodeValueKeywordWidget::on_ValueEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setValue(qPrintable(ui_.ValueEdit->text()));
    checkValueValidity();

    emit(keywordValueChanged(keyword_->optionMask()));
}

void NodeValueKeywordWidget::on_ValueEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setValue(qPrintable(ui_.ValueEdit->text()));
    checkValueValidity();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Check validity of current value
void NodeValueKeywordWidget::checkValueValidity() { ui_.ValueValidIndicator->setOK(keyword_->data().isValid()); }

// Update value displayed in widget
void NodeValueKeywordWidget::updateValue()
{
    refreshing_ = true;

    ui_.ValueEdit->setText(QString::fromStdString(keyword_->data().asString()));

    refreshing_ = false;
}
