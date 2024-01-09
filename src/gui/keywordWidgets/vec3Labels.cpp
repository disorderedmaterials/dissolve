// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/vec3Labels.h"

// Set/hide/show label according to labelling type
void Vec3WidgetLabels::set(QLabel *label, Vec3Labels::LabelType labelType, int index)
{
    // Show / hide label
    label->setVisible(labelType != Vec3Labels::NoLabels);

    // Set label
    switch (labelType)
    {
        case (Vec3Labels::NoLabels):
            label->setText("");
            break;
        case (Vec3Labels::ABCLabels):
            if (index == 0)
                label->setText("A");
            else if (index == 1)
                label->setText("B");
            else if (index == 2)
                label->setText("C");
            break;
        case (Vec3Labels::AlphaBetaGammaLabels):
            if (index == 0)
                label->setText(QString::fromUtf8("\u03b1"));
            else if (index == 1)
                label->setText(QString::fromUtf8("\u03b2"));
            else if (index == 2)
                label->setText(QString::fromUtf8("\u03b3"));
            break;
        case (Vec3Labels::MinMaxDeltaLabels):
            if (index == 0)
                label->setText("Min");
            else if (index == 1)
                label->setText("Max");
            else if (index == 2)
                label->setText("Delta");
            break;
        case (Vec3Labels::MinMaxBinwidthlabels):
            if (index == 0)
                label->setText("Min");
            else if (index == 1)
                label->setText("Max");
            else if (index == 2)
                label->setText("Bin Width");
            break;
        case (Vec3Labels::HKLLabels):
            if (index == 0)
                label->setText("H");
            else if (index == 1)
                label->setText("K");
            else if (index == 2)
                label->setText("L");
            break;
        case (Vec3Labels::XYZLabels):
            if (index == 0)
                label->setText("X");
            else if (index == 1)
                label->setText("Y");
            else if (index == 2)
                label->setText("Z");
            break;
    }
}
