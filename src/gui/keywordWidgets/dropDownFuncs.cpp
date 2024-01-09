// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/dropDown.h"

KeywordDropDown::KeywordDropDown(QWidget *parent)
{
    ui_.setupUi(this);

    // Connect signals/slots
    connect(&dropWidget_, SIGNAL(hidden()), this, SLOT(dropWidgetHidden()));
}

/*
 * Drop Widget
 */

// Return the drop widget
DropWidget *KeywordDropDown::dropWidget() { return &dropWidget_; }

/*
 * Signals / Slots
 */

void KeywordDropDown::on_CallDropWidgetButton_clicked(bool checked)
{
    if (!checked)
        dropWidget_.hide();
    else
    {
        QPoint popupPos;
        if (parentWidget())
            popupPos = parentWidget()->mapToGlobal(pos() + QPoint(0, height()));
        dropWidget_.show();
        dropWidget_.move(popupPos);
    }
}

void KeywordDropDown::dropWidgetHidden()
{
    // Un-check the call button
    ui_.CallDropWidgetButton->setChecked(false);

    // Need to store data from widget, in case it has changed
    updateKeywordData();
}

/*
 * Update
 */

// Set summary text on call button
void KeywordDropDown::setSummaryText(const QString s) { ui_.CallDropWidgetButton->setText(s); }

// Set icon on call button
void KeywordDropDown::setSummaryIcon(QIcon icon) { ui_.CallDropWidgetButton->setIcon(icon); }

// Update keyword data based on widget values
void KeywordDropDown::updateKeywordData() {}