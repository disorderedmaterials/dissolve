// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "gui/maintab.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "module/module.h"
#include <QFrame>
#include <QInputDialog>
#include <QLayout>
#include <QMdiArea>
#include <QMdiSubWindow>

MainTab::MainTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title, QWidget *page)
    : dissolve_(dissolve)
{
    dissolveWindow_ = dissolveWindow;
    tabWidget_ = parent;
    page_ = page;
    title_ = title;
}

MainTab::~MainTab() {}

// Return enum options for TabType
EnumOptions<MainTab::TabType> MainTab::tabTypes()
{
    static EnumOptionsList TabTypeOptions = EnumOptionsList() << EnumOption(MainTab::ConfigurationTabType, "ConfigurationTab")
                                                              << EnumOption(MainTab::ForcefieldTabType, "ForcefieldTab")
                                                              << EnumOption(MainTab::LayerTabType, "LayerTab")
                                                              << EnumOption(MainTab::SpeciesTabType, "SpeciesTab")
                                                              << EnumOption(MainTab::WorkspaceTabType, "WorkspaceTab");

    static EnumOptions<MainTab::TabType> options("TabType", TabTypeOptions);

    return options;
}

/*
 * Data
 */

// Raise suitable dialog for entering / checking new tab name
QString MainTab::getNewTitle(bool &ok)
{
    return QInputDialog::getText(page_, "Rename Tab", "Enter the new name for the tab", QLineEdit::Normal, title_, &ok);
}

// Return page widget
QWidget *MainTab::page() const { return page_; }

// Return title of tab
const QString MainTab::title() const { return title_; }

/*
 * Management
 */

// Return whether the title of the tab can be changed
bool MainTab::canChangeTitle() const { return false; }

// Rename tab through suitable dialog / widget
bool MainTab::rename()
{
    if (!canChangeTitle())
        return false;

    // Find ourselves in the tab widget
    auto tabIndex = tabWidget_->indexOf(page_);
    if (tabIndex == -1)
    {
        Messenger::print("Couldn't rename tab '{}' since its page widget could not be found.\n", qPrintable(title_));
        return false;
    }

    // Get the new name
    bool ok;
    QString text = getNewTitle(ok);
    if (!ok)
        return false;

    title_ = qPrintable(text);
    tabWidget_->setTabText(tabIndex, text);

    return true;
}

// Return whether the tab can be closed (after any necessary user querying, etc.)
bool MainTab::canClose() const { return true; }
