// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/keywordwidgets/producers.h"
#include "gui/keywordwidgets/widget.hui"
#include "gui/signals.h"
#include "main/dissolve.h"
#include "module/module.h"
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

KeywordsWidget::KeywordsWidget(QWidget *parent) : QWidget(parent)
{
    refreshing_ = false;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

/*
 * Controls
 */

// Set up controls for specified keywords
void KeywordsWidget::setUp(std::string_view groupName, const KeywordStore::KeywordStoreMap &keywordMap,
                           const CoreData &coreData)
{
    keywordWidgets_.clear();

    // Create a new QWidget and layout for the next group?
    auto *groupWidget = new QWidget;
    auto *groupLayout = new QGridLayout(groupWidget);
    auto row = 0;
    for (auto &[sectionName, keywords] : keywordMap.at(groupName))
    {
        // Create a widget for the section name
        if (!sectionName.empty())
        {
            auto *sectionLabel = new QLabel(QString::fromStdString(std::string(sectionName)));
            groupLayout->addWidget(sectionLabel, row++, 0, 1, 2);
        }

        // Loop over keywords in the group and add them to our groupbox
        for (auto *keyword : keywords)
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

            // Put the widget in a horizontal layout with a stretch to absorb extra space
            auto *w = new QHBoxLayout;
            w->addWidget(widget, 0);
            w->addStretch(1);

            // Create a label and add it and the widget to our layout
            auto *nameLabel = new QLabel(QString::fromStdString(std::string(keyword->name())));
            nameLabel->setToolTip(QString::fromStdString(std::string(keyword->description())));
            groupLayout->addWidget(nameLabel, row, 0);
            groupLayout->addLayout(w, row++, 1);

            // Push onto our reference list
            keywordWidgets_.push_back(base);
        }
    }

    // Add vertical spacer to the end of the group
    groupLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
    setLayout(groupLayout);
}

// Create a suitable button for the named group
std::pair<QPushButton *, bool> KeywordsWidget::buttonForGroup(std::string_view groupName)
{
    // Create basic button
    auto *b = new QPushButton(QString::fromStdString(std::string(groupName)));
    b->setCheckable(true);
    b->setAutoExclusive(true);
    b->setFlat(true);

    const std::vector<std::tuple<std::string_view, QString, bool>> knownButtons = {
        {"Options", ":/general/icons/general_options.svg", false},
        {"Export", ":/menu/icons/menu_save.svg", false},
        {"Advanced", ":/general/icons/general_advanced.svg", true},
    };

    // Apply icons / alignment to recognised buttons
    bool alignRight = false;
    auto it = std::find_if(knownButtons.begin(), knownButtons.end(),
                           [groupName](const auto &btnData) { return std::get<0>(btnData) == groupName; });
    if (it != knownButtons.end())
    {
        b->setIcon(QIcon(std::get<1>(*it)));
        alignRight = std::get<2>(*it);
    }

    return {b, alignRight};
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
