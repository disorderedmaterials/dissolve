// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/widget.h"

ModuleWidget::ModuleWidget(QWidget *parent, Dissolve &dissolve) : QWidget(parent), dissolve_(dissolve), refreshing_(false) {}

/*
 * UI
 */

// Update controls within widget
void ModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags) {}

// Disable editing within widget
void ModuleWidget::preventEditing() {}

// Allow editing within widget
void ModuleWidget::allowEditing() {}
