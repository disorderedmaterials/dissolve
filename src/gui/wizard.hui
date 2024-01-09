// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_wizardFooter.h"
#include "gui/ui_wizardHeader.h"
#include <QDialog>
#include <QStackedWidget>
#include <optional>

// Wizard Dialog
class WizardDialog : public QDialog
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    WizardDialog(QWidget *parent);
    ~WizardDialog() = default;

    /*
     * Widgets
     */
    private:
    // Stack widget containing wizard pages
    QStackedWidget *stack_;
    // Header form declaration
    Ui::WizardHeader headerUi_;
    // Footer form declaration
    Ui::WizardFooter footerUi_;
    // Whether header controls are available
    bool headerAvailable_;
    // Whether the close button is available
    bool closeButtonAvailable_;
    // Whether footer controls are available
    bool footerAvailable_;

    protected:
    // Set up dialog, attaching header and footer to placeholders, and setting the initial page
    void initialise(QWidget *widget, QStackedWidget *stack, int startIndex);
    // Set icon in header
    void setWizardHeaderIcon(QString iconResource);

    /*
     * Page Management
     */
    public:
    // Page Types
    enum class PageType
    {
        Normal,
        Choice
    };
    // Page Type
    using WizardPage = std::tuple<PageType, int, QString, std::optional<int>>;

    private:
    // List of pages that we know about
    std::vector<WizardPage> pages_;
    // History of traversed page indices
    std::vector<int> history_;

    public:
    // Register normal page
    void registerPage(int index, QString title, std::optional<int> nextIndex = std::nullopt);
    // Register choice page (no Finish / Next buttons)
    void registerChoicePage(int index, QString title);
    // Return whether page with specified index exists
    bool hasPage(int index);
    // Get page with specified index
    const WizardPage &getPage(int index);

    /*
     * Update
     */
    protected:
    // Update controls in header and footer widgets to reflect the specified page
    void updateHeaderAndFooter(const WizardPage &page);

    /*
     * Wizard
     */
    private:
    // Current page ID
    std::optional<int> currentPageID_;

    protected:
    // Go to specified page index in the widget
    bool displayPage(int index);
    // Return current page index
    std::optional<int> currentPage() const;
    // Return whether progression to the next page from the current page is allowed
    virtual bool progressionAllowed(int index) const;
    // Perform any necessary actions before moving to the next page
    virtual bool prepareForNextPage(int currentIndex);
    // Determine next page for the current page, based on current data
    virtual std::optional<int> determineNextPage(int currentIndex);
    // Perform any necessary actions before moving to the previous page
    virtual bool prepareForPreviousPage(int currentIndex);
    // Perform any final actions before the wizard is closed
    virtual void finalise();

    protected Q_SLOTS:
    // Update state of progression ('Next' / 'Finish') controls
    bool updateProgressionControls();
    // Go to previous page
    void goToPreviousPage(bool checked);
    // Go to previous page
    void goToPreviousPage();
    // Go to next page
    void goToNextPage(bool checked);
    // Go to next page
    void goToNextPage();
    // End the wizard
    void finishWizard(bool checked);
    // Close the wizard
    void closeWizard(bool checked);

    public:
    // Display specified page in controls widget
    void goToPage(int index);
    // Go back one page in the history
    void goBack();
};
