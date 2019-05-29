/*
	*** Guide Page
	*** src/gui/guidepage.h
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

#ifndef DISSOLVE_GUIDEPAGE_H
#define DISSOLVE_GUIDEPAGE_H

#include "gui/wizardwidgethighlight.h"
#include "gui/guidepagehighlight.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/listitem.h"

// Forward Declarations
class QWidget;

// Guide Page
class GuidePage : public ListItem<GuidePage>
{
	public:
	// Constructor / Destructor
	GuidePage();
	~GuidePage();


	/*
	 * Basic Definition
	 */
	private:
	// Name / title text for page
	CharString name_;
	// Resource location for icon
	CharString iconUrl_;
	// Whether this page is a Finish point
	bool finishPoint_;
	// Name of the page to move to once 'Next' is clicked
	CharString nextPageName_;
	// Pointer to the page to move to once 'Next' is clicked
	GuidePage* nextPage_;

	public:
	// Set page name
	void setName(const char* name);
	// Return name of page
	const char* name() const;
	// Set resource location for icon
	void setIconUrl(const char* resourceUrl);
	// Return resource location for icon
	const char* iconUrl() const;
	// Return whether an icon url has been set
	bool hasIconUrl() const;
	// Set whether this page is a Finish point
	void setFinishPoint(bool b);
	// Whether this page is a Finish point
	bool finishPoint() const;
	// Set name for the page following this one
	void setNextPageName(const char* name);
	// Return name for the page following this one
	const char* nextPageName() const;
	// Return whether a next page name has been set
	bool hasNextPageName() const;
	// Set pointer to the page to move to once 'Next' is clicked
	void setNextPage(GuidePage* page);
	// Pointer to the page to move to once 'Next' is clicked
	GuidePage* nextPage() const;


	/*
	 * Page Content
	 */
	private:
	// Rich text content for the page
	QString richTextContent_;

	public:
	// Set rich text content for the page
	void setRichTextContent(QString richText);
	// Return rich text content for the page
	QString richTextContent() const;


	/*
	 * Interaction
	 */
	private:
	// Highlighted widgets
	List<GuidePageHighlight> highlights_;

	public:
	// Add new widget highlight
	GuidePageHighlight* addHighlight();
	// Return highlighted widgets list
	const List<GuidePageHighlight>& highlights() const;
};

#endif
