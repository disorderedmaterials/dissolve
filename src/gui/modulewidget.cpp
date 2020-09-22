// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "gui/modulewidget.h"

ModuleWidget::ModuleWidget(QWidget *parent) : QWidget(parent) { refreshing_ = false; }

ModuleWidget::~ModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void ModuleWidget::updateControls(int flags) {}

// Disable sensitive controls within widget
void ModuleWidget::disableSensitiveControls() {}

// Enable sensitive controls within widget
void ModuleWidget::enableSensitiveControls() {}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool ModuleWidget::writeState(LineParser &parser) const { return true; }

// Read widget state through specified LineParser
bool ModuleWidget::readState(LineParser &parser) { return true; }
