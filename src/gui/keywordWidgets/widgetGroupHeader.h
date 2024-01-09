// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/ui_widgetGroupHeader.h"
#include <QWidget>

class WidgetGroupHeader : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    WidgetGroupHeader(const QString &groupName, const QString &description = {});

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::GroupHeaderWidget ui_;
};
