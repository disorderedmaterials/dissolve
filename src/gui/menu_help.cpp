// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "gui/gui.h"
#include <QDesktopServices>

void DissolveWindow::on_HelpOnlineManualAction_triggered(bool checked)
{
    QDesktopServices::openUrl(QUrl("https://www.projectdissolve.com/docs/userguide/"));
}

void DissolveWindow::on_HelpOnlineTutorialsAction_triggered(bool checked)
{
    QDesktopServices::openUrl(QUrl("https://www.projectdissolve.com/docs/examples/"));
}
