/*
	*** Get Workspace Name Dialog
	*** src/gui/gettabnamedialog_funcs.cpp
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

#include "gui/gettabnamedialog.h"
#include "gui/maintab.h"
#include "base/sysfunc.h"

// Constructor
GetTabNameDialog::GetTabNameDialog(QWidget* parent, RefList<const MainTab> currentTabs)
{
	ui_.setupUi(this);

	currentTabs_ = currentTabs;
}

// Destructor
GetTabNameDialog::~GetTabNameDialog()
{
}

// Run the dialog
bool GetTabNameDialog::get(const MainTab* currentTab, const char* currentName)
{
	currentTab_ = currentTab;

	// Set the current text
	ui_.NameEdit->setText(currentName);

	show();

	return (exec() == QDialog::Accepted);
}

// Return the name string
QString GetTabNameDialog::newName() const
{
	return ui_.NameEdit->text();
}

/*
 * Slots
 */

// Workspace name edited
void GetTabNameDialog::on_NameEdit_textChanged(const QString text)
{
	// Make sure the name is valid
	bool nameValid = true;
	if (text.isEmpty()) nameValid = false;
	else
	{
		for (const MainTab* tab : currentTabs_)
		{
			if (currentTab_ == tab) continue;

			if (DissolveSys::sameString(tab->title(), qPrintable(text)))
			{
				nameValid = false;
				break;
			}
		}
	}

	// Update indicator
	ui_.NameIndicator->setOK(nameValid);

	// Update dialog buttons
	ui_.OKButton->setEnabled(nameValid);
}

void GetTabNameDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void GetTabNameDialog::on_OKButton_clicked(bool checked)
{
	accept();
}
