/*
	*** Guide Wizard Functions
	*** src/gui/guidewizard_funcs.cpp
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

#include "gui/guidewizard.h"
#include "gui/gui.h"
#include "gui/guide.h"
#include "main/dissolve.h"

// Constructor / Destructor
GuideWizard::GuideWizard(QWidget* parent)
{
	dissolveReference_ = NULL;
	dissolveWindow_ = NULL;

	// Set up our UI, and attach the wizard's widgets to our vacant QFrame
	ui_.setupUi(this);
	setUpHeaderAndFooter(this);

	startPageIndex_ = -1;
}

GuideWizard::~GuideWizard()
{
}

/*
 * Data
 */

// Set Dissolve reference
void GuideWizard::setMainDissolveReference(const Dissolve* dissolveReference)
{
	dissolveReference_ = dissolveReference;
}

// Set main GUI window pointer
void GuideWizard::setDissolveWindow(DissolveWindow* dissolveWindow)
{
	dissolveWindow_ = dissolveWindow;
}

/*
 * Controls
 */

// Go to specified page index in the controls widget
bool GuideWizard::displayControlPage(int index)
{
	// Find page with given index
	WizardWidgetPageInfo* page = findPage(index);
	if (!page) return Messenger::error("Page index %i not recognised.\n", index);

	// Page index is valid, so show the related widget
	if (!page->stackedWidgetPage()) return Messenger::error("Page index %i does not have an associated stack page widget.\n", index);
	ui_.MainStack->setCurrentWidget(page->stackedWidgetPage());

	// Highlight any widgets that need it
	page->highlightWidgets();

	return true;
}

// Return whether progression to the next page from the current page is allowed
bool GuideWizard::progressionAllowed(int index) const
{
	return true;
}

// Perform any necessary actions before moving to the next page
bool GuideWizard::prepareForNextPage(int currentIndex)
{
	// Find page with given index
	WizardWidgetPageInfo* page = findPage(currentIndex);
	if (!page) return Messenger::error("Page index %i not recognised.\n", currentIndex);

	// Un-highlight any widgets that were previously highlighted
	page->unHighlightWidgets();

	return true;
}


// Determine next page for the current page, based on current data
int GuideWizard::determineNextPage(int currentIndex)
{
	// Find page with given index
	WizardWidgetPageInfo* page = findPage(currentIndex);
	if (!page) return Messenger::error("Page index %i not recognised.\n", currentIndex);

	// TODO

	return -1;
}

// Perform any necessary actions before moving to the previous page
bool GuideWizard::prepareForPreviousPage(int currentIndex)
{
	// Find page with given index
	WizardWidgetPageInfo* page = findPage(currentIndex);
	if (!page) return Messenger::error("Page index %i not recognised.\n", currentIndex);

	// Un-highlight any widgets that were previously highlighted
	page->unHighlightWidgets();

	return true;
}

/*
 * Controls
 */

// Reset widget, ready for running the wizard again
void GuideWizard::reset()
{
	// Reset the underlying WizardWidget
	resetToPage(startPageIndex_);
}

/*
 * Content Management
 */

// Clear current content
void GuideWizard::clear()
{
	// Remove all page widgets
	while (ui_.MainStack->count() > 0)
	{
		QWidget* page = ui_.MainStack->widget(0);
		ui_.MainStack->removeWidget(page);
		page->deleteLater();
	}

	// Clear the underlying pages
	clearPages();
}

// Set up specified guide content
bool GuideWizard::setContent(const Guide& guide)
{
	// Clear any existing content
	clear();
	
	// Loop over defined pages in the Guide, adding WizardWidgetPageInfo's for each
	ListIterator<GuidePage> pageIterator(guide.pages());
	while (GuidePage* page = pageIterator.iterate())
	{
		// Create a new widget for the page
		QWidget* widget = new QWidget;
		QVBoxLayout* layout = new QVBoxLayout;
		layout->setContentsMargins(4,4,4,4);
		layout->setSpacing(4);
		QLabel* label = new QLabel;
		label->setWordWrap(true);
		label->setFont(QFont("SourceSansPro-Regular", 12));
		label->setText(page->richTextContent());
		label->setAlignment(Qt::AlignTop);
		layout->addWidget(label);
		widget->setLayout(layout);
		ui_.MainStack->addWidget(widget);

		// Create a pageinfo reference
		WizardWidgetPageInfo* pageInfo = addPage();
		pageInfo->setStackedWidgetPage(widget);

		pageInfo->setTitle(page->name());
		pageInfo->setIndex(guide.indexOfPage(page->name()));
		if (page->hasNextPageName()) pageInfo->setNextPageIndex(guide.indexOfPage(page->nextPageName()));

		// Add any page highlights
		ListIterator<GuidePageHighlight> highlightIterator(page->highlights());
		while (GuidePageHighlight* highlight = highlightIterator.iterate())
		{
			// Attempt to find the widget first
			QWidget* highlightWidget = dissolveWindow_->findChild<QWidget*>(highlight->widgetName());
			if (!highlightWidget)
			{
				Messenger::error("Can't find highlight widget '%s' in main Dissolve window.", highlight->widgetName());
				continue;
			}

			// Add a new highlight
			WizardWidgetHighlight* wh = pageInfo->addWidgetHighlight();

			wh->setWidget(highlightWidget);
			wh->setColour(highlight->colour());
		}
	};

	// Set start page index
	startPageIndex_ = guide.indexOfPage(guide.startPageName());

	// Set wizard header icon (if set)
	if (guide.hasIconUrl()) setWizardHeaderIcon(guide.iconUrl());

	// Reset the wizard to its starting page
	reset();

	return true;
}
