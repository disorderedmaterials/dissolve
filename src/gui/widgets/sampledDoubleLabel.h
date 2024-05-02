// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/widgets/ui_sampledDoubleLabel.h"

// Forward Declarations
class SampledDouble;

// Sampled Double Display Label
class SampledDoubleLabel : public QLabel
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    SampledDoubleLabel(QWidget *parent);
    ~SampledDoubleLabel();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SampledDoubleLabel ui_;

    public:
    // Set font sizes for labels
    void setLabelFonts(int basePointSize);
    // Set label values
    void setText(const SampledDouble &sampledDouble);
};
