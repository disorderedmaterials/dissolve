// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/keywordwidgets/sectionHeader.h"

SectionHeaderWidget::SectionHeaderWidget(QString sectionLabel) : QWidget()
{
    // Setup our UI
    ui_.setupUi(this);

    ui_.SectionLabel->setText(sectionLabel);
}
