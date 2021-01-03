// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/sq/gui/modulewidget.h"
#include "modules/sq/sq.h"

// Return a new widget controlling this Module
ModuleWidget *SQModule::createWidget(QWidget *parent, Dissolve &dissolve) { return new SQModuleWidget(parent, this, dissolve); }
