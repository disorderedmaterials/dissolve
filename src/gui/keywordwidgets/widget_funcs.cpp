// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/keywordwidgets/producers.h"
#include "gui/keywordwidgets/widget.hui"
#include "gui/signals.h"
#include "main/dissolve.h"
#include "module/module.h"
#include <QFormLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QToolBox>

KeywordsWidget::KeywordsWidget(QWidget *parent) : QTabWidget(parent)
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
        removeTab(0);
    keywordWidgets_.clear();

    // Get the organisation info from the keyword store
    std::string_view currentGroupName = "";
    QWidget *groupWidget = nullptr;
    QGridLayout *groupLayout = nullptr;
    auto row = 0;
    auto &&[keywordIndex, keywordMap] = keywords.keywordOrganisation();
    for (auto &[groupName, sectionName] : keywordIndex)
    {
        // Check current group name widget we're processing
        if ((currentGroupName != groupName) && groupWidget)
        {
            // Add a vertical spacer to finish the group
            groupLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
            addTab(groupWidget, QString::fromStdString(std::string(currentGroupName)));
            groupWidget = nullptr;
        }

        currentGroupName = groupName;

        // Create a new QWidget and layout for the next group?
        if (!groupWidget)
        {
            groupWidget = new QWidget;
            groupLayout = new QGridLayout(groupWidget);
            row = 0;
        }

        // Create a widget for the section name
        if (sectionName != "")
        {
            auto *sectionLabel = new QLabel(QString::fromStdString(std::string(sectionName)));
            groupLayout->addWidget(sectionLabel, row++, 0, 1, 2);
        }

        // Loop over keywords in the group and add them to our groupbox
        for (auto *keyword : keywordMap[groupName][sectionName])
        {
            // Try to create a suitable widget
            auto [widget, base] = KeywordWidgetProducer::create(keyword, coreData);
            if (!widget || !base)
            {
                fmt::print("No widget created for keyword '{}'.\n", keyword->name());
                continue;
            }

            // Connect signals
            connect(widget, SIGNAL(keywordDataChanged(int)), this, SLOT(keywordDataChanged(int)));

            // Create a label and add it and the widget to our layout
            auto *nameLabel = new QLabel(QString::fromStdString(std::string(keyword->name())));
            nameLabel->setToolTip(QString::fromStdString(std::string(keyword->description())));
            groupLayout->addWidget(nameLabel, row, 0);
            groupLayout->addWidget(widget, row++, 1);

            // Push onto our reference list
            keywordWidgets_.push_back(base);
        }
    }

    // Add final group
    if (groupWidget)
        addTab(groupWidget, QString::fromStdString(std::string(currentGroupName)));
}

// Update controls within widget
void KeywordsWidget::updateControls(int dataMutationFlags)
{
    refreshing_ = true;

    Flags<DissolveSignals::DataMutations> mutationFlags(dataMutationFlags);

    // Update all our keyword widgets
    for (auto *keywordWidget : keywordWidgets_)
        keywordWidget->updateValue(mutationFlags);

    refreshing_ = false;
}

/*
 * Signals / Slots
 */

// Keyword data changed
void KeywordsWidget::keywordDataChanged(int keywordSignalMask) { emit(keywordChanged(keywordSignalMask)); }
