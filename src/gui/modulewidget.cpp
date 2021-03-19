// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/modulewidget.h"

ModuleWidget::ModuleWidget(QWidget *parent, const GenericList &processingData)
    : QWidget(parent), processingData_(processingData), refreshing_(false)
{
}

ModuleWidget::~ModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void ModuleWidget::updateControls(UpdateType updateType) {}

// Disable sensitive controls within widget
void ModuleWidget::disableSensitiveControls() {}

// Enable sensitive controls within widget
void ModuleWidget::enableSensitiveControls() {}
