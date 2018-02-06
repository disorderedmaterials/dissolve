/*
	*** Drop-Down for Keyword Widget
	*** src/gui/keywordwidgets/dropdown_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of DUQ.

	DUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	DUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with DUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/keywordwidgets/dropdown.h"

// Constructor
KeywordDropDown::KeywordDropDown(QWidget* parent)
{
	ui.setupUi(this);

	// Connect signals/slots
	connect(&dropWidget_, SIGNAL(hidden()), this, SLOT(dropWidgetHidden()));
}

/*
 * Drop Widget
 */

// Return the drop widget
DropWidget* KeywordDropDown::dropWidget()
{
	return &dropWidget_;
}

/*
 * Signals / Slots
 */

void KeywordDropDown::on_CallDropWidgetButton_clicked(bool checked)
{
	if (!checked) dropWidget_.hide();
	else
	{
		QPoint popupPos;
		if (parentWidget()) popupPos = parentWidget()->mapToGlobal(pos()+QPoint(0, height()));
		dropWidget_.show();
		dropWidget_.move(popupPos);
	}
}

void KeywordDropDown::dropWidgetHidden()
{
	// Un-check the call button
	ui.CallDropWidgetButton->setChecked(false);

	// Need to store data from widget, in case it has changed
	updateKeywordData();
}

/*
 * Update
 */

// Set summary text on call button
void KeywordDropDown::setSummaryText(const char* text)
{
	ui.CallDropWidgetButton->setText(text);
}

