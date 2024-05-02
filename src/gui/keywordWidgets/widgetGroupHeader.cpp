// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/widgetGroupHeader.h"

WidgetGroupHeader::WidgetGroupHeader(const QString &groupName, const QString &description) : QWidget()
{
    // Setup our UI
    ui_.setupUi(this);

    ui_.GroupLabel->setText(groupName);
    if (description.isEmpty())
        ui_.DescriptionLabel->setVisible(false);
    else
        ui_.DescriptionLabel->setText(description);
}
