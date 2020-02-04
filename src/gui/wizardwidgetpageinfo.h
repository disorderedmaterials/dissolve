/*
	*** Wizard Widget Page Info
	*** src/gui/wizardwidgetpageinfo.h
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

#ifndef DISSOLVE_WIZARDWIDGETPAGEINFO_H
#define DISSOLVE_WIZARDWIDGETPAGEINFO_H

#include "gui/wizardwidgethighlight.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/listitem.h"

// Forward Declarations
class QWidget;

// Wizard Page Info
class WizardWidgetPageInfo : public ListItem<WizardWidgetPageInfo>
{
	public:
	// Constructor / Destructor
	WizardWidgetPageInfo();
	~WizardWidgetPageInfo();
	// Page Types
	enum PageType { NormalPage, ChoicePage, FinishPage };


	/*
	 * Data
	 */
	private:
	// Page type
	PageType pageType_;
	// Identifying index for this page
	int index_;
	// Title text for page
	CharString title_;
	// Resource location for icon
	CharString iconUrl_;
	// Index of the 'next' page
	int nextIndex_;

	public:
	// Set page information
	void set(int index, const char* title, int nextIndex = -1);
	// Set page index
	void setIndex(int index);
	// Set page title
	void setTitle(const char* title);
	// Set next page index
	void setNextPageIndex(int nextIndex);
	// Set page type
	void setPageType(WizardWidgetPageInfo::PageType type);
	// Return page type
	WizardWidgetPageInfo::PageType pageType() const;
	// Return identifying index for page
	int index() const;
	// Return title text for page
	const char* title() const;
	// Return index of the 'next' page
	int nextIndex() const;
	// Return whether this is an exit (finish) point
	bool isFinishPoint() const;


	/*
	 * Widget Interaction
	 */
	private:
	// Associated widget in QStackedWidget
	QWidget* stackedWidgetPage_;
	// Highlighted widgets
	List<WizardWidgetHighlight> widgetHighlights_;

	public:
	// Set Associated widget in QStackedWidget
	void setStackedWidgetPage(QWidget* widget);
	// Return associated widget in QStackedWidget
	QWidget* stackedWidgetPage() const;
	// Add new widget highlight
	WizardWidgetHighlight* addWidgetHighlight();
	// Highlight all defined widgets
	void highlightWidgets();
	// Un-highlight all defined widgets
	void unHighlightWidgets();
};

#endif
