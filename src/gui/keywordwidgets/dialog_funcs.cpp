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

	connect(ui_.Keywords, SIGNAL(dataModified()), this, SLOT(keywordChanged()));
	connect(ui_.Keywords, SIGNAL(setUpRequired()), this, SLOT(setUpRequired()));

	keywordsModified_ = false;
	setUpRequired_ = false;
}

// Destructor
KeywordsDialog::~KeywordsDialog()
{
}

// Run the dialog
void KeywordsDialog::showOptions()
{
	keywordsModified_ = false;
	setUpRequired_ = false;

	exec();
}

// Return whether any keywords have been modified in the current 'show'
bool KeywordsDialog::keywordsModified() const
{
	return keywordsModified_;
}

// Return whether any set-up needs to be re-run following keyword modification
bool KeywordsDialog::isSetUpRequired() const
{
	return setUpRequired_;
}

/*
 * Slots
 */

void KeywordsDialog::keywordChanged()
{
	keywordsModified_ = true;
}

void KeywordsDialog::setUpRequired()
{
	setUpRequired_ = true;
}

void KeywordsDialog::on_OKButton_clicked(bool checked)
{
	accept();
}
