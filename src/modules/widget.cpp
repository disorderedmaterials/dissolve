// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/widget.h"

ModuleWidget::ModuleWidget(QWidget *parent, Dissolve &dissolve) : QWidget(parent), dissolve_(dissolve), refreshing_(false) {}

/*
 * UI
 */

// Update controls within widget
void ModuleWidget::updateControls(UpdateType updateType) {}

// Disable sensitive controls within widget
void ModuleWidget::disableSensitiveControls() {}

// Enable sensitive controls within widget
void ModuleWidget::enableSensitiveControls() {}
