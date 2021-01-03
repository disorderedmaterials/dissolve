// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/wizardwidgethighlight.h"
#include "templates/list.h"
#include "templates/listitem.h"

// Forward Declarations
class QWidget;

// Wizard Page Info
class WizardWidgetPageInfo : public ListItem<WizardWidgetPageInfo>
{
    public:
    WizardWidgetPageInfo();
    ~WizardWidgetPageInfo();
    // Page Types
    enum PageType
    {
        FinishHereFlag = -99,
        NormalPage = 0,
        ChoicePage = 1
    };

    /*
     * Data
     */
    private:
    // Page type
    PageType pageType_;
    // Identifying index for this page
    int index_;
    // Title text for page
    QString title_;
    // Resource location for icon
    QString iconUrl_;
    // Index of the 'next' page (for linear progressions)
    int nextIndex_;

    public:
    // Set page information
    void set(int index, const QString title, int nextIndex = -1);
    // Set page index
    void setIndex(int index);
    // Set page title
    void setTitle(const QString title);
    // Set next page index
    void setNextPageIndex(int nextIndex);
    // Set page type
    void setPageType(WizardWidgetPageInfo::PageType type);
    // Return page type
    WizardWidgetPageInfo::PageType pageType() const;
    // Return identifying index for page
    int index() const;
    // Return title text for page
    const QString title() const;
    // Return index of the 'next' page
    int nextIndex() const;
    // Return whether this is an exit (finish) point
    bool isFinishPoint() const;

    /*
     * Widget Interaction
     */
    private:
    // Associated widget in QStackedWidget
    QWidget *stackedWidgetPage_;
    // Highlighted widgets
    List<WizardWidgetHighlight> widgetHighlights_;

    public:
    // Set Associated widget in QStackedWidget
    void setStackedWidgetPage(QWidget *widget);
    // Return associated widget in QStackedWidget
    QWidget *stackedWidgetPage() const;
    // Add new widget highlight
    WizardWidgetHighlight *addWidgetHighlight();
    // Highlight all defined widgets
    void highlightWidgets();
    // Un-highlight all defined widgets
    void unHighlightWidgets();
};
