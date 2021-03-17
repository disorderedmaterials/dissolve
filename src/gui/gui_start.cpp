// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/gui.h"
#include "main/dissolve.h"

/*
 * Create
 */

void DissolveWindow::on_StartCreateNewButton_clicked(bool checked) { ui_.FileNewAction->trigger(); }

void DissolveWindow::on_StartCreateFromTemplateButton_clicked(bool checked) { ui_.FileNewFromTemplateAction->trigger(); }

/*
 * Open / Connect
 */

void DissolveWindow::on_StartOpenLocalButton_clicked(bool checked) { ui_.FileOpenLocalAction->trigger(); }

void DissolveWindow::on_StartOpenRecentButton_clicked(bool checked) { ui_.FileOpenRecentAction->trigger(); }

void DissolveWindow::on_StartConnectButton_clicked(bool checked) { ui_.FileConnectAction->trigger(); }

/*
 * Help
 */

void DissolveWindow::on_StartOnlineManualButton_clicked(bool checked) { ui_.HelpOnlineManualAction->trigger(); }

void DissolveWindow::on_StartOnlineTutorialsButton_clicked(bool checked) { ui_.HelpOnlineTutorialsAction->trigger(); }
