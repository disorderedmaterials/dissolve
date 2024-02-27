// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/wizard.h"
#include <fmt/format.h>
#include <stdexcept>

WizardDialog::WizardDialog(QWidget *parent)
    : QDialog(parent), headerAvailable_(false), closeButtonAvailable_(true), footerAvailable_(false),
      currentPageID_(std::nullopt)
{
}

/*
 * Widgets
 */

// Set up dialog, attaching header and footer to placeholders, and setting the initial page
void WizardDialog::initialise(QWidget *widget, QStackedWidget *stack, int startIndex)
{
    stack_ = stack;

    // Search for the named QWidgets 'WizardHeaderWidget' and 'WizardFooterWidget'
    auto *headerWidget = widget->findChild<QWidget *>("WizardHeaderWidget");
    if (headerWidget)
    {
        headerAvailable_ = true;

        headerUi_.setupUi(headerWidget);
    }

    auto *footerWidget = widget->findChild<QWidget *>("WizardFooterWidget");
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

    goToPage(startIndex);
}

// Set icon in header
void WizardDialog::setWizardHeaderIcon(QString iconResource)
{
    if (headerAvailable_)
        headerUi_.IconLabel->setPixmap(QPixmap(iconResource));
}

// Update controls in header and footer widgets to reflect the specified page
void WizardDialog::updateHeaderAndFooter(const WizardPage &page)
{
    auto [pageType, index, title, nextIndex] = page;

    // Set header widgets
    if (headerAvailable_)
        headerUi_.TitleLabel->setText(std::get<2>(page));

    // Set footer widgets
    if (footerAvailable_)
    {
        // Set status of buttons
        footerUi_.BackButton->setEnabled(history_.size() > 1);
        if (!nextIndex.has_value())
            nextIndex = determineNextPage(index);
        footerUi_.NextButton->setVisible(pageType == WizardDialog::PageType::Normal && nextIndex.has_value());
        footerUi_.NextButton->setEnabled(nextIndex.has_value());
        footerUi_.FinishButton->setVisible(!nextIndex.has_value());

        // Disable controls if we're not allowed to progress yet
        auto allowProgression = progressionAllowed(index);
        footerUi_.FinishButton->setEnabled(allowProgression);
        footerUi_.NextButton->setEnabled(allowProgression);
    }
}

/*
 * Page Management
 */

// Register normal page
void WizardDialog::registerPage(int index, QString title, std::optional<int> nextIndex)
{
    assert(!hasPage(index));

    if (!currentPageID_.has_value())
        currentPageID_ = index;

    pages_.emplace_back(WizardDialog::PageType::Normal, index, title, nextIndex);
}

// Register choice page (no Finish / Next buttons)
void WizardDialog::registerChoicePage(int index, QString title)
{
    assert(!hasPage(index));

    if (!currentPageID_.has_value())
        currentPageID_ = index;

    pages_.emplace_back(WizardDialog::PageType::Choice, index, title, std::nullopt);
}

// Return whether page with specified index exists
bool WizardDialog::hasPage(int index)
{
    return std::find_if(pages_.begin(), pages_.end(), [index](const auto &page) { return std::get<1>(page) == index; }) !=
           pages_.end();
}

// Get page with specified index
const WizardDialog::WizardPage &WizardDialog::getPage(int index)
{
    auto it = std::find_if(pages_.begin(), pages_.end(), [index](const auto &page) { return std::get<1>(page) == index; });
    assert(it != pages_.end());
    return *it;
}

/*
 * Wizard
 */

// Go to specified page index in the widget
bool WizardDialog::displayPage(int index)
{
    assert(index >= 0 && index < stack_->count());

    stack_->setCurrentIndex(index);

    return true;
}

// Return current page index
std::optional<int> WizardDialog::currentPage() const { return currentPageID_; }

// Return whether progression to the next page from the current page is allowed
bool WizardDialog::progressionAllowed(int index) const { return true; }

// Perform any necessary actions before moving to the next page
bool WizardDialog::prepareForNextPage(int currentIndex) { return true; }

// Determine next page for the current page, based on current data
std::optional<int> WizardDialog::determineNextPage(int currentIndex) { return std::nullopt; }

// Perform any necessary actions before moving to the previous page
bool WizardDialog::prepareForPreviousPage(int currentIndex) { return true; }

// Perform any final actions before the wizard is closed
void WizardDialog::finalise() {}

// Update state of progression ('Next' / 'Finish') controls
bool WizardDialog::updateProgressionControls()
{
    if (!currentPageID_.has_value())
        return false;

    updateHeaderAndFooter(getPage(currentPageID_.value()));

    return true;
}

// Go to previous page
void WizardDialog::goToPreviousPage(bool checked) { goToPreviousPage(); }

// Go to previous page
void WizardDialog::goToPreviousPage()
{
    if (!currentPageID_.has_value())
        return;

    // Perform any necessary actions before moving to the previous page
    if (!prepareForPreviousPage(currentPageID_.value()))
        return;

    // Move to the previous page in the history
    goBack();
}

// Go to next page
void WizardDialog::goToNextPage(bool checked) { goToNextPage(); }

// Go to next page
void WizardDialog::goToNextPage()
{
    if (!currentPageID_.has_value())
        return;

    // Perform any necessary actions before moving to the next page
    if (!prepareForNextPage(currentPageID_.value()))
        return;

    auto &page = getPage(currentPageID_.value());

    // Move to the next page defined for the current page
    auto nextIndex = std::get<3>(page);
    if (!nextIndex.has_value())
        nextIndex = determineNextPage(currentPageID_.value());

    // If we still have no valid index, complain!
    if (!nextIndex.has_value())
        throw(std::runtime_error("No valid Next page could be determined.\n"));
    else
        goToPage(nextIndex.value());
}

// End the wizard
void WizardDialog::finishWizard(bool checked)
{
    finalise();

    accept();
}

// Close the wizard
void WizardDialog::closeWizard(bool checked) { reject(); }

// Display specified page in controls widget
void WizardDialog::goToPage(int index)
{
    assert(hasPage(index));
    currentPageID_ = index;

    // Instruct the derived class to switch pages
    if (!displayPage(index))
        throw(std::runtime_error(fmt::format("Failed to go to and display page id {}.\n", index)));

    // Add this page to our history
    history_.push_back(index);

    // Update the header and footer
    updateHeaderAndFooter(getPage(index));
}

// Go back one page in the history
void WizardDialog::goBack()
{
    // We cannot go back further than the first item in history_, so if there is only one item in the history_ do nothing
    assert(history_.size() > 1);

    // Remove the last item from the history_ (our current page)
    history_.pop_back();

    // Get the page now on the top of the history_ list
    currentPageID_ = history_.back();

    // Instruct the derived class to switch pages
    if (!displayPage(currentPageID_.value()))
        throw(std::runtime_error(fmt::format("Failed to go back to and display page id {}.\n", currentPageID_.value())));

    // Update the header and footer
    updateHeaderAndFooter(getPage(currentPageID_.value()));
}
