// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/messagesTab.h"
#include <QFontDatabase>
#include <QScrollBar>

MessagesTab::MessagesTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title)
    : MainTab(dissolveWindow, dissolve, parent, title, this)
{
    ui_.setupUi(this);

    ui_.MessagesEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    // Link output handler in to the Messenger
    Messenger::setOutputHandler(&outputHandler_);
    connect(&outputHandler_, SIGNAL(printText(const QString &)), this, SLOT(appendMessage(const QString &)));
    connect(&outputHandler_, SIGNAL(setColour(const QColor &)), ui_.MessagesEdit, SLOT(setTextColor(const QColor &)));
}

/*
 * UI
 */

void MessagesTab::on_MessagesIncreaseFontSizeButton_clicked(bool checked)
{
    QFont font = ui_.MessagesEdit->font();
    font.setPointSize(font.pointSize() + 1);
    ui_.MessagesEdit->setFont(font);
}

void MessagesTab::on_MessagesDecreaseFontSizeButton_clicked(bool checked)
{
    QFont font = ui_.MessagesEdit->font();
    font.setPointSize(font.pointSize() - 1);
    ui_.MessagesEdit->setFont(font);
}

void MessagesTab::clearMessages() { ui_.MessagesEdit->clear(); }

void MessagesTab::appendMessage(const QString &msg)
{
    ui_.MessagesEdit->verticalScrollBar()->setSliderPosition(ui_.MessagesEdit->verticalScrollBar()->maximum());

    ui_.MessagesEdit->insertPlainText(msg);
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType MessagesTab::type() const { return MainTab::TabType::Messages; }

/*
 * Update
 */

// Update controls in tab
void MessagesTab::updateControls() {}

// Prevent editing within tab
void MessagesTab::preventEditing() {}

// Allow editing within tab
void MessagesTab::allowEditing() {}
