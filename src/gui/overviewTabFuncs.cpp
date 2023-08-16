// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/overviewTab.h"
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>

OverviewTab::OverviewTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title)
    : MainTab(dissolveWindow, dissolve, parent, title, this), dissolveModel_(dissolve)
{
    // Create a layout
    QHBoxLayout *topLeftLayout = new QHBoxLayout(this);

    // Create the view
    view_ = new QQuickWidget(this);
    // We need to do this before loading the actual QML
    view_->rootContext()->setContextProperty("dissolveModel", QVariant::fromValue(&dissolveModel_));
    // Load the QML, and listen for the status of the view to be changed (i.e. for it to be ready)
    view_->setSource(QUrl("qrc:/dialogs/qml/OverviewTab.qml"));
    connect(view_, SIGNAL(statusChanged(QQuickWidget::Status)), SLOT(viewStatusChanged()));

    // Add the view to the widget, and center it
    topLeftLayout->addWidget(view_);
    topLeftLayout->setAlignment(view_, Qt::AlignHCenter);
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType OverviewTab::type() const { return MainTab::TabType::Overview; }

/*
 * Update
 */

// Update controls in tab
void OverviewTab::updateControls() { dissolveModel_.reset(); }

// Prevent editing within tab
void OverviewTab::preventEditing() {}

// Allow editing within tab
void OverviewTab::allowEditing() {}

/*
 * Slots
 */

// Status of the QML view changed
void OverviewTab::viewStatusChanged()
{
    if (view_->status() == QQuickWidget::Status::Ready && !slotsAreSetup_)
    {
        // If the view is ready, then connect our slots to its signals
        connect(view_->rootObject(), SIGNAL(nodeClicked(int, int)), this, SLOT(nodeClicked(int, int)));
        slotsAreSetup_ = true;
    }
}

// Node has been clicked
void OverviewTab::nodeClicked(int row, int col)
{
    auto obj = dissolveModel_.rawData(dissolveModel_.index(row, col));
    auto *sp = obj.value<Species *>();
    auto *cfg = obj.value<Configuration *>();
    if (sp)
        tabWidget_->setCurrentTab(sp);
    else if (cfg)
        tabWidget_->setCurrentTab(cfg);
    else
    {
        tabWidget_->setCurrentIndex(1);
        auto *tab = dynamic_cast<ForcefieldTab *>(tabWidget_->currentTab());
        tab->setTabIndex(std::min(col, 1));
    }
}