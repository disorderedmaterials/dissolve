/*
	*** TMenuButtonGroup - Group of TMenuButton with custom handling of exclusivity
	*** src/gui/widgets/tmenubuttongroup.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_TMENUBUTTONGROUP_H
#define DISSOLVE_TMENUBUTTONGROUP_H

#include "templates/listitem.h"
#include "templates/reflist.h"
#include <QString>

// Forward Declarations (Qt)
class TMenuButton;

// Exclusive menu button group
class TMenuButtonGroup : public ListItem<TMenuButtonGroup>
{
	public:
	// Constructor
	TMenuButtonGroup();

	private:
	// Name of group
	QString name_;
	// List of TMenuButtons (or TMenuButtons) in this group
	RefList<TMenuButton,int> buttons_;

	public:
	// Set name of group
	void setName(QString name);
	// Return name of group
	QString name();
	// Add button to group
	void addButton(TMenuButton* button);
	// Set specified button as checked button
	void setCurrentButton(TMenuButton* button);
	// Set button with specified text as checked button
	bool setCurrentButton(QString buttonText);
	// Set button with specified index as checked button
	bool setCurrentButton(int buttonIndex);
};

#endif

