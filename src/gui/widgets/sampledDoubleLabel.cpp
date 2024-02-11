// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/widgets/sampledDoubleLabel.h"
#include "math/sampledDouble.h"

SampledDoubleLabel::SampledDoubleLabel(QWidget *parent) : QLabel(parent) { ui_.setupUi(this); }

SampledDoubleLabel::~SampledDoubleLabel() {}

/*
 * UI
 */

// Set font sizes for labels
void SampledDoubleLabel::setLabelFonts(int basePointSize)
{
    QFont newFont = font();

    newFont.setPointSize(basePointSize);
    ui_.ValueLabel->setFont(newFont);

    newFont.setPointSizeF(0.8 * basePointSize);
    ui_.SeparatorLabel->setFont(newFont);
    ui_.StDevLabel->setFont(newFont);
}

// Set label values
void SampledDoubleLabel::setText(const SampledDouble &sampledDouble)
{
    ui_.ValueLabel->setText(QString::number(sampledDouble.value()));
    ui_.StDevLabel->setText(QString::number(sampledDouble.stDev()));
}
