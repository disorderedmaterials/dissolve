/*
	*** System Template
	*** src/gui/systemtemplate.h
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

#ifndef DISSOLVE_SYSTEMTEMPLATE_H
#define DISSOLVE_SYSTEMTEMPLATE_H

#include "templates/listitem.h"
#include <QDir>
#include <QString>

// Forward Declarations
class QToolButton;

// System Template
class SystemTemplate : public ListItem<SystemTemplate>
{
	public:
	// Constructor / Destructor
	SystemTemplate();
	~SystemTemplate();
	// Copy Constructor
	SystemTemplate(const SystemTemplate& source);
	// Assignment operator
	void operator=(const SystemTemplate& source);


	/*
	 * Data
	 */
	private:
	// Name of the template
	QString name_;
	// Group in which the template exists
	QString group_;
	// Short description of the template
	QString description_;
	// Resource path to the template's icon
	QString iconResource_;
	// Template's input data
	QString inputFileData_;
	// QToolButton created for the template
	QToolButton* toolButton_;

	public:
	// Read data contained in specified dir
	bool read(const QDir rootResourceDir);
	// Return name of the template
	QString name() const;
	// Return group in which the template exists
	QString group() const;
	// Return short description of the template
	QString description() const;
	// Return resource path to the template's icon
	QString iconResource() const;
	// Return the template's input data
	QString inputFileData() const;
	// Return QToolButton created for the template
	QToolButton* toolButton() const;
	// Create and return a button for this template
	QToolButton* createButton();
};

#endif
