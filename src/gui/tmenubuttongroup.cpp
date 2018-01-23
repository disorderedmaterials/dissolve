/*
	*** TMenuButtonGroup Functions
	*** src/gui/tmenubuttongroup_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/tmenubuttongroup.h"
#include "gui/tmenubutton.hui"
#include "gui/gui.h"
#include <QButtonGroup>

// Constructor
TMenuButtonGroup::TMenuButtonGroup() : ListItem<TMenuButtonGroup>()
{
	name_ = "UnnamedGroup";
}

// Set name of group
void TMenuButtonGroup::setName(QString name)
{
	name_ = name;
}

// Return name of group
QString TMenuButtonGroup::name()
{
	return name_;
}

// Add button to group
void TMenuButtonGroup::addButton(TMenuButton* button)
{
	buttons_.add(button);
}

// Set specified button as checked button
void TMenuButtonGroup::setCurrentButton(TMenuButton* button)
{
	// Loop over buttons in group, unchecking all others except the one provided (which we will check)
	TMenuButton* groupButton;
	for (RefListItem<TMenuButton,int>* ri = buttons_.first(); ri != NULL; ri = ri->next)
	{
		groupButton = ri->item;
		if (groupButton == button) groupButton->setChecked(true);
		else
		{
			groupButton->setChecked(false);
			groupButton->setDown(false);
		}
	}
}

// Set button with specified text as checked button
bool TMenuButtonGroup::setCurrentButton(QString buttonText)
{
	// Loop over buttons in group, unchecking all others except the one provided (which we will check)
	TMenuButton* groupButton;
	for (RefListItem<TMenuButton,int>* ri = buttons_.first(); ri != NULL; ri = ri->next)
	{
		groupButton = ri->item;
		if (groupButton->text() == buttonText)
		{
			setCurrentButton(groupButton);
			return true;
		}
	}
	return false;
}

// Set button with specified index as checked button
bool TMenuButtonGroup::setCurrentButton(int buttonIndex)
{
	// Loop over buttons in group, unchecking all others except the one provided (which we will check)
	TMenuButton* groupButton;
	for (RefListItem<TMenuButton,int>* ri = buttons_.first(); ri != NULL; ri = ri->next)
	{
		groupButton = ri->item;
		if (groupButton->index() == buttonIndex)
		{
			setCurrentButton(groupButton);
			return true;
		}
	}
	return false;
}

