/*
	*** Keywords Dialog
	*** src/gui/keywordwidgets/dialog_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/keywordwidgets/dialog.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/sysfunc.h"

// Constructor
KeywordsDialog::KeywordsDialog(QWidget* parent, const KeywordList& keywords, const CoreData& coreData) : coreData_(coreData)
{
	ui_.setupUi(this);

	ui_.Keywords->setUp(keywords, coreData);
}

// Destructor
KeywordsDialog::~KeywordsDialog()
{
}

// Run the dialog, returning whether any modifications to keyword values were made
bool KeywordsDialog::showOptions()
{
	keywordsModified_ = false;

	exec();

	return keywordsModified_;
}

/*
 * Slots
 */

void KeywordsDialog::keywordChanged()
{
	keywordsModified_ = true;
}

void KeywordsDialog::on_OKButton_clicked(bool checked)
{
	accept();
}
