// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "keywords/store.h"
#include <QScrollArea>

// Forward Declarations
class CoreData;
class QPushButton;

// Keywords Widget
class KeywordsWidget : public QScrollArea
{
    Q_OBJECT

    public:
    KeywordsWidget(QWidget *parent = nullptr);
    ~KeywordsWidget() = default;

    /*
     * Controls
     */
    private:
    // Whether the widget is currently refreshing
    bool refreshing_;
    // Reference vector of displayed keyword widgets
    std::vector<KeywordWidgetBase *> keywordWidgets_;

    public:
    // Set up controls for specified keywords
    void setUp(const KeywordStoreSection &keywordSection, CoreData &coreData);
    // Create a suitable button for the named group
    static std::pair<QPushButton *, bool> buttonForGroup(std::string_view groupName);

    public Q_SLOTS:
    // Update controls within widget
    void updateControls(int dataMutationFlags = 0);

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    // Keyword data changed
    void keywordDataChanged(int keywordSignalMask);

    Q_SIGNALS:
    // Keyword has been modified
    void keywordChanged(int keywordSignalMask);
};
