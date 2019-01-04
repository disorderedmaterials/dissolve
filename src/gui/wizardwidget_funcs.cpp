/*
	*** Wizard Widget Functions
	*** src/gui/wizardwidget_funcs.cpp
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

#include "gui/wizardwidget.hui"

// Constructor / Destructor
WizardWidget::WizardWidget()
{
	headerAvailable_ = false;
	footerAvailable_ = false;
	currentPage_ = NULL;
}

WizardWidget::~WizardWidget()
{
}

// Attach header and footer to existing named widgets in specified widget
void WizardWidget::setUpHeaderAndFooter(QWidget* widget)
{
	// Search for the named QWidgets 'WizardHeaderWidget' and 'WizardFooterWidgetr'
	QWidget* headerWidget = widget->findChild<QWidget*>("WizardHeaderWidget");
	if (headerWidget)
	{
		headerAvailable_ = true;

		headerUi_.setupUi(headerWidget);
	}
	else printf("Header widget not found.\n");

	QWidget* footerWidget = widget->findChild<QWidget*>("WizardFooterWidget");
	if (footerWidget)
	{
		footerAvailable_ = true;

		footerUi_.setupUi(footerWidget);

		// Connect signals / slots
		connect(footerUi_.BackButton, SIGNAL(clicked(bool)), this, SLOT(backButtonClicked(bool)));
		connect(footerUi_.NextButton, SIGNAL(clicked(bool)), this, SLOT(nextButtonClicked(bool)));
		connect(footerUi_.FinishButton, SIGNAL(clicked(bool)), this, SLOT(finishButtonClicked(bool)));
	}
	else printf("Footer widget not found.\n");
}

// Update controls in header and footer widgets to reflect the specified page
void WizardWidget::updateHeaderAndFooter(WizardWidgetPageInfo* page)
{
	// Set header widgets
	if (headerAvailable_)
	{
		headerUi_.TitleLabel->setText(page ? page->title() : "???PAGE");
	}

	// Set footer widgets
	if (footerAvailable_)
	{
		// Set status of buttons
		footerUi_.BackButton->setEnabled(history_.nItems() > 1);
		if (page)
		{
			bool allowNextOrFinish = progressionAllowed(page->index());
			footerUi_.NextButton->setVisible(page->pageType() != WizardWidgetPageInfo::FinishPage);
			footerUi_.NextButton->setEnabled((page->pageType() == WizardWidgetPageInfo::NormalPage) && (page->nextIndex() != -1) && allowNextOrFinish);
			footerUi_.FinishButton->setVisible(page->pageType() == WizardWidgetPageInfo::FinishPage);
			footerUi_.FinishButton->setEnabled(allowNextOrFinish);
		}
		else
		{
			footerUi_.NextButton->setVisible(false);
			footerUi_.FinishButton->setVisible(false);
			footerUi_.FinishButton->setEnabled(false);
		}
	}
}

/*
 * Page Management
 */

// Find page with specified index
WizardWidgetPageInfo* WizardWidget::findPage(int index)
{
	ListIterator<WizardWidgetPageInfo> pageIterator(pages_);
	while (WizardWidgetPageInfo* page = pageIterator.iterate()) if (page->index() == index) return page;

	return NULL;
}

// Register page
WizardWidgetPageInfo* WizardWidget::registerPage(int index, const char* title, int nextIndex)
{
	// Check that the specified index isn't already registered...
	WizardWidgetPageInfo* page = findPage(index);
	if (page) printf("Internal Error: Page with index %i has already been registered.\n", index);
	else page = pages_.add();

	// Set page info
	page->set(index, title, nextIndex);

	return page;
}

// Register choice page (no Finish / Next buttons)
void WizardWidget::registerChoicePage(int index, const char* title)
{
	WizardWidgetPageInfo* page = registerPage(index, title);

	page->setPageType(WizardWidgetPageInfo::ChoicePage);
}

// Register exit (finish) page
void WizardWidget::registerFinishPage(int index, const char* title)
{
	WizardWidgetPageInfo* page = registerPage(index, title);

	page->setPageType(WizardWidgetPageInfo::FinishPage);
}

/*
 * Control
 */

// Update state of progression ('Next' / 'Finish') controls
bool WizardWidget::updateProgressionControls()
{
	updateHeaderAndFooter(currentPage_);
}

// Back button clicked
void WizardWidget::backButtonClicked(bool checked)
{
	if (!currentPage_) return;

	// Perform any necessary actions before moving to the previous page
	if (!prepareForPreviousPage(currentPage_->index())) return;

	// Move to the previous page in the history
	goBack();
}

// Next button clicked
void WizardWidget::nextButtonClicked(bool checked)
{
	if (!currentPage_) return;

	// Perform any necessary actions before moving to the next page
	if (!prepareForNextPage(currentPage_->index())) return;

	// Move to the next page defined for the current page
	if (currentPage_) goToPage(currentPage_->nextIndex());
}

// Finish button clicked
void WizardWidget::finishButtonClicked(bool checked)
{
}

// Reset wizard and begin again from specified page
void WizardWidget::resetToPage(int index)
{
	// Clear the current page history
	history_.clear();

	// Display the specified page
	goToPage(index);
}

// Display specified page in controls widget
void WizardWidget::goToPage(int index)
{
	// Find the page with the specified index
	currentPage_ = findPage(index);
	if (!currentPage_)
	{
		updateHeaderAndFooter(NULL);
		return;
	}

	// Instruct the derived class to switch pages
	if (!displayControlPage(index))
	{
		updateHeaderAndFooter(NULL);
		return;
	}

	// Add this page to our history
	history_.add(currentPage_);

	// Update the header and footer
	updateHeaderAndFooter(currentPage_);
}

// Go back one page in the history
void WizardWidget::goBack()
{
	// We cannot go back further than the first item in history_, so if there is only one itfinishEnabledem in the history_ do nothing
	if (history_.nItems() == 1)
	{
		printf("Can't go back further than the first page visited.\n");
		return;
	}

	// Remove the last item from the history_ (our current page)
	history_.removeLast();

	// Get the page now on the top of the history_ list
	currentPage_ = history_.lastItem();
	if (!currentPage_)
	{
		updateHeaderAndFooter(NULL);
		return;
	}

	// Instruct the derived class to switch pages
	if (!displayControlPage(currentPage_->index()))
	{
		updateHeaderAndFooter(NULL);
		return;
	}

	// Update the header and footer
	updateHeaderAndFooter(currentPage_);
}
