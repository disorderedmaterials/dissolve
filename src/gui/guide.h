/*
	*** Guide for GuideWizard
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

// Guide for display in a GuideWizard
class Guide : public ListItem<Guide>
{
	public:
	// Constructor / Destructor
	Guide();
	~Guide();
	// Clear all data
	void clear();


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
	CharString startPageName_;
	// Pointer to start page
	GuidePage* startPage_;

	public:
	// Return index of page with supplied name
	int indexOfPage(const char* name) const;
	// Return GuidePage with name specified
	GuidePage* page(const char* name);
	// Return name of intended start page
	const char* startPageName() const;
	// Return list of defined pages
	const List<GuidePage>& pages() const;


	/*
	 * I/O
	 */
	private:
	// Resolve internal links between pages, converting tags to pointers
	void resolveInternalLinks();

	public:
	// Load guide data from specified resource
	bool load(const char* resourceName);
	// Save guide data to specified resource
	bool save(const char* resourceName);
};

#endif
