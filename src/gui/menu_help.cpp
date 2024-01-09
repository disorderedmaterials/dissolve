// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/gui.h"
#include <QDesktopServices>
#include <QUrl>

void DissolveWindow::on_HelpOnlineManualAction_triggered(bool checked)
{
    QDesktopServices::openUrl(QUrl("https://www.projectdissolve.com/docs/"));
}

void DissolveWindow::on_HelpOnlineTutorialsAction_triggered(bool checked)
{
    QDesktopServices::openUrl(QUrl("https://www.projectdissolve.com/examples/"));
}
