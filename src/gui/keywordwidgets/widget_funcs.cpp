// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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

// Create widget for specified keyword
QWidget *KeywordsWidget::createKeywordWidget(RefList<KeywordWidgetBase> &keywordWidgets, KeywordBase *keyword,
                                             const CoreData &coreData)
{
    // Try to create a suitable widget
    auto [w, base] = KeywordWidgetProducer::create(keyword, coreData);
    if (!w || !base)
        return nullptr;

    // Connect signals
    connect(w, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));

    // Set tooltip on widget (using the description from the keyword pointer passed, rather than its base)
    w->setToolTip(QString::fromStdString(std::string(keyword->description())));

    // Add to the list of widgets
    keywordWidgets.append(base);

    return w;
}

// Set up controls for specified keywords
void KeywordsWidget::setUp(KeywordStore &keywords, const CoreData &coreData)
{
    // Clear existing item groups....
    while (count() > 0)
        removeItem(0);
    keywordWidgets_.clear();

    for (auto &[groupName, keyNames] : keywords.displayGroups())
    {
        // Create a new QWidget and layout for our widgets
        QWidget *groupWidget = new QWidget;
        QFormLayout *groupLayout = new QFormLayout(groupWidget);

        // Loop over keywords in the group and add them to our groupbox
        for (const auto &keyName : keyNames)
        {
            // Find the keyword in the main map
            auto *keyword = keywords.find(keyName);
            if (!keyword)
                throw(std::runtime_error(
                    fmt::format("Keyword '{}' is listed in group '{}', but it could not be found.\n", keyName, groupName)));

            // Create / setup the keyword widget
            auto *widget = createKeywordWidget(keywordWidgets_, keyword, coreData);

            if (!widget)
            {
                Messenger::printVerbose("No widget created for keyword '{}'.\n", keyword->name());
                continue;
            }

            // Create a label and add it and the widget to our layout
            QLabel *nameLabel = new QLabel(QString::fromStdString(std::string(keyword->name())));
            nameLabel->setToolTip(QString::fromStdString(std::string(keyword->description())));
            groupLayout->addRow(nameLabel, widget);
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
    for (KeywordWidgetBase *keywordWidget : keywordWidgets_)
        keywordWidget->updateValue();

    refreshing_ = false;
}

/*
 * Signals / Slots
 */

// Keyword data changed
void KeywordsWidget::keywordDataChanged(int flags)
{
    // Always emit the 'dataModified' signal
    emit(dataModified());

    // Set-up of encompassing class required?
    if (flags & KeywordBase::ModificationRequiresSetUpOption)
        emit(setUpRequired());
}
