/*
	*** Guide for GeneralWizard
	*** src/gui/guide.h
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

#ifndef DISSOLVE_GUIDE_H
#define DISSOLVE_GUIDE_H

#include "gui/guidepage.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/listitem.h"

// Forward Declarations
class QWidget;
class QXmlStreamReader;

// Guide for display in a GeneralWizard
class Guide : public ListItem<Guide>
{
	public:
	// Constructor / Destructor
	Guide();
	~Guide();


	/*
	 * Description
	 */
	private:
	// Name of guide
	CharString name_;
	// Resource location for icon
	CharString iconUrl_;
	// Whether guide is valid
	bool valid_;

	public:
	// Return name of the guide
	const char* name() const;
	// Return resource location for icon
	const char* iconUrl() const;
	// Return whether an icon url has been set
	bool hasIconUrl() const;
	// Return whether the guide is valid
	bool isValid() const;


	/*
	 * Page Data
	 */
	private:
	// List of defined pages
	List<GuidePage> pages_;
	// Tag for start page
	CharString startPageTag_;

	public:
	// Return index of page with supplied tag
	int indexOfTag(const char* tag) const;
	// Return tag for start page
	const char* startPageTag() const;
	// Return list of defined pages
	const List<GuidePage>& pages() const;


	/*
	 * I/O
	 */
	public:
	// Load guide data from specified resource
	bool load(const char* resourceName);
};

#endif
