/*
    *** Wizard Widget Functions
    *** src/gui/wizardwidget_funcs.cpp
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

#include "base/sysfunc.h"
#include "gui/wizardwidget.hui"

WizardWidget::WizardWidget()
{
    headerAvailable_ = false;
    footerAvailable_ = false;
    closeButtonAvailable_ = true;

    currentPage_ = nullptr;
}

WizardWidget::~WizardWidget() {}

/*
 * Widgets
 */

// Attach header and footer to existing named widgets in specified widget
void WizardWidget::setUpHeaderAndFooter(QWidget *widget)
{
    // Search for the named QWidgets 'WizardHeaderWidget' and 'WizardFooterWidgetr'
    QWidget *headerWidget = widget->findChild<QWidget *>("WizardHeaderWidget");
    if (headerWidget)
    {
        headerAvailable_ = true;

        headerUi_.setupUi(headerWidget);

        connect(headerUi_.CloseButton, SIGNAL(clicked(bool)), this, SLOT(closeWizard(bool)));
    }
    else
        Messenger::error("Header widget not found.\n");

    QWidget *footerWidget = widget->findChild<QWidget *>("WizardFooterWidget");
    if (footerWidget)
    {
        footerAvailable_ = true;

        footerUi_.setupUi(footerWidget);

        // Connect signals / slots
        connect(footerUi_.CancelButton, SIGNAL(clicked(bool)), this, SLOT(closeWizard(bool)));
        connect(footerUi_.BackButton, SIGNAL(clicked(bool)), this, SLOT(goToPreviousPage(bool)));
        connect(footerUi_.NextButton, SIGNAL(clicked(bool)), this, SLOT(goToNextPage(bool)));
        connect(footerUi_.FinishButton, SIGNAL(clicked(bool)), this, SLOT(finishWizard(bool)));
    }
    else
        Messenger::error("Footer widget not found.\n");
}

// Set icon in header
void WizardWidget::setWizardHeaderIcon(QString iconResource)
{
    if (headerAvailable_)
        headerUi_.IconLabel->setPixmap(QPixmap(iconResource));
}

// Update controls in header and footer widgets to reflect the specified page
void WizardWidget::updateHeaderAndFooter(WizardWidgetPageInfo *page)
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
            auto nextIndex = page->nextIndex();
            if (nextIndex == -1)
                nextIndex = determineNextPage(page->index());
            footerUi_.NextButton->setVisible((page->pageType() == WizardWidgetPageInfo::NormalPage) &&
                                             (nextIndex != WizardWidgetPageInfo::FinishHereFlag));
            footerUi_.NextButton->setEnabled(nextIndex != -1);
            footerUi_.FinishButton->setVisible(nextIndex == WizardWidgetPageInfo::FinishHereFlag);

            // Disable controls if we're not allowed to progress yet
            auto allowProgression = progressionAllowed(page->index());
            footerUi_.FinishButton->setEnabled(allowProgression);
            footerUi_.NextButton->setEnabled(allowProgression);
        }
        else
        {
            footerUi_.NextButton->setVisible(false);
            footerUi_.FinishButton->setVisible(false);
            footerUi_.FinishButton->setEnabled(false);
        }
    }
}

// Set whether the close button is available
void WizardWidget::setCloseButtonAvailable(bool b)
{
    closeButtonAvailable_ = b;

    if (headerAvailable_)
        headerUi_.CloseButton->setVisible(closeButtonAvailable_);
}

// Return whether header controls are available
bool WizardWidget::headerAvailable() const { return headerAvailable_; }

// Return whether footer controls are available
bool WizardWidget::footerAvailable() const { return footerAvailable_; }

/*
 * Page Management
 */

// Clear all registered pages
void WizardWidget::clearPages()
{
    pages_.clear();
    history_.clear();
    currentPage_ = nullptr;

    updateHeaderAndFooter(nullptr);
}

// Add empty page
WizardWidgetPageInfo *WizardWidget::addPage()
{
    WizardWidgetPageInfo *page = pages_.add();
    page->setIndex(pages_.nItems());

    return page;
}

// Register page
WizardWidgetPageInfo *WizardWidget::registerPage(int index, QString title, int nextIndex)
{
    // Check that the specified index isn't already registered...
    WizardWidgetPageInfo *page = findPage(index);
    if (page)
        Messenger::error("Page with index {} has already been registered.\n", index);
    else
        page = pages_.add();

    // Set page info
    page->set(index, title, nextIndex);

    return page;
}

// Register choice page (no Finish / Next buttons)
void WizardWidget::registerChoicePage(int index, QString title)
{
    WizardWidgetPageInfo *page = registerPage(index, title);

    page->setPageType(WizardWidgetPageInfo::ChoicePage);
}

// Find page with specified index
WizardWidgetPageInfo *WizardWidget::findPage(int index)
{
    ListIterator<WizardWidgetPageInfo> pageIterator(pages_);
    while (WizardWidgetPageInfo *page = pageIterator.iterate())
        if (page->index() == index)
            return page;

    return nullptr;
}

/*
 * Control
 */

// Update state of progression ('Next' / 'Finish') controls
bool WizardWidget::updateProgressionControls()
{
    updateHeaderAndFooter(currentPage_);

    return true;
}

// Go to previous page
void WizardWidget::goToPreviousPage(bool checked) { goToPreviousPage(); }

// Go to previous page
void WizardWidget::goToPreviousPage()
{
    if (!currentPage_)
        return;

    // Perform any necessary actions before moving to the previous page
    if (!prepareForPreviousPage(currentPage_->index()))
        return;

    // Move to the previous page in the history
    goBack();
}

// Go to next page
void WizardWidget::goToNextPage(bool checked) { goToNextPage(); }

// Go to next page
void WizardWidget::goToNextPage()
{
    if (!currentPage_)
        return;

    // Perform any necessary actions before moving to the next page
    if (!prepareForNextPage(currentPage_->index()))
        return;

    // Move to the next page defined for the current page
    auto nextIndex = currentPage_->nextIndex();
    if (nextIndex == -1)
        nextIndex = determineNextPage(currentPage_->index());

    // If we still have no valid index, complain!
    if (nextIndex == -1)
        Messenger::error("No valid Next page could be determined.\n");
    else
        goToPage(nextIndex);
}

// End the wizard
void WizardWidget::finishWizard(bool checked) { emit(finished()); }

// Close the wizard
void WizardWidget::closeWizard(bool checked) { emit(canceled()); }

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
        updateHeaderAndFooter(nullptr);
        return;
    }

    // Instruct the derived class to switch pages
    if (!displayControlPage(index))
    {
        updateHeaderAndFooter(nullptr);
        return;
    }

    // Add this page to our history
    history_.append(currentPage_);

    // Update the header and footer
    updateHeaderAndFooter(currentPage_);
}

// Go back one page in the history
void WizardWidget::goBack()
{
    // We cannot go back further than the first item in history_, so if there is only one item in the history_ do nothing
    if (history_.nItems() == 1)
    {
        Messenger::warn("Can't go back further than the first page visited.\n");
        return;
    }

    // Remove the last item from the history_ (our current page)
    history_.removeLast();

    // Get the page now on the top of the history_ list
    currentPage_ = history_.lastItem();
    if (!currentPage_)
    {
        updateHeaderAndFooter(nullptr);
        return;
    }

    // Instruct the derived class to switch pages
    if (!displayControlPage(currentPage_->index()))
    {
        updateHeaderAndFooter(nullptr);
        return;
    }

    // Update the header and footer
    updateHeaderAndFooter(currentPage_);
}
