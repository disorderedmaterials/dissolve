// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/controlLayerTab.h"
#include "gui/mainTab.h"

ControlLayerTab::ControlLayerTab(DissolveWindow *dissolveWindow, Dissolve& dissolve, MainTabsWidget *parent, const QString title) : MainTab(dissolveWindow, dissolve, parent, QString("Control"), this), controlProcedure_(ProcedureNode::ControlContext, "Control")
{
    ui_.setupUi(this);
    ui_.ControlWidget->setUp(dissolveWindow_, controlProcedure_);
}

MainTab::TabType ControlLayerTab::type() const { return MainTab::TabType::ControlLayer; }
bool ControlLayerTab::canClose() const {return false;}
void ControlLayerTab::updateControls() {}
void ControlLayerTab::preventEditing() {}
void ControlLayerTab::allowEditing() {}

void ControlLayerTab::on_RunButton_clicked(bool checked) {}
