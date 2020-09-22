// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/test/gui/modulewidget.h"
#include "modules/test/test.h"

// Return a new widget controlling this Module
ModuleWidget *TestModule::createWidget(QWidget *parent, Dissolve &dissolve) { return new TestModuleWidget(parent, this); }
