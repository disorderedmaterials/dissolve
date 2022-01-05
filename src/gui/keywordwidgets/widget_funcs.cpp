// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/keywordwidgets/producers.h"
#include "gui/keywordwidgets/widget.hui"
#include "main/dissolve.h"
#include "module/module.h"
#include <QFormLayout>
#include <QLabel>
#include <QToolBox>

KeywordsWidget::KeywordsWidget(QWidget *parent) : QToolBox(parent)
{
    refreshing_ = false;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

/*
 * Controls
 */

// Set up controls for specified keywords
void KeywordsWidget::setUp(KeywordStore &keywords, const CoreData &coreData)
{
    // Clear existing item groups....
    while (count() > 0)
        removeItem(0);
    keywordWidgets_.clear();

    for (auto &[groupName, keywords] : keywords.displayGroups())
    {
        // Create a new QWidget and layout for our widgets
        auto *groupWidget = new QWidget;
        auto *groupLayout = new QFormLayout(groupWidget);

        // Loop over keywords in the group and add them to our groupbox
        for (const auto &keyword : keywords)
        {
            // Try to create a suitable widget
            auto [widget, base] = KeywordWidgetProducer::create(keyword, coreData);
            if (!widget || !base)
            {
                Messenger::printVerbose("No widget created for keyword '{}'.\n", keyword->name());
                continue;
            }

            // Connect signals
            connect(widget, SIGNAL(keywordDataChanged(int)), this, SLOT(keywordDataChanged(int)));

            // Create a label and add it and the widget to our layout
            auto *nameLabel = new QLabel(QString::fromStdString(std::string(keyword->name())));
            nameLabel->setToolTip(QString::fromStdString(std::string(keyword->description())));
            groupLayout->addRow(nameLabel, widget);

            // Push onto our reference list
            keywordWidgets_.push_back(base);
        }

        // Group is finished, so add the widget as a new tab in our QToolBox
        addItem(groupWidget, QString::fromStdString(std::string(groupName)));
    }
}

// Update controls within widget
void KeywordsWidget::updateControls()
{
    refreshing_ = true;

    // Update all our keyword widgets
    for (auto *keywordWidget : keywordWidgets_)
        keywordWidget->updateValue();

    refreshing_ = false;
}

/*
 * Signals / Slots
 */

// Keyword data changed
void KeywordsWidget::keywordDataChanged(int keywordSignalMask) { emit(keywordChanged(keywordSignalMask)); }
