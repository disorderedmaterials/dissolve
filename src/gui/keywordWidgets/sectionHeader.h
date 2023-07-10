// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/ui_sectionHeader.h"
#include <QWidget>

class SectionHeaderWidget : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SectionHeaderWidget(QString sectionLabel);

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::SectionHeaderWidget ui_;
};
