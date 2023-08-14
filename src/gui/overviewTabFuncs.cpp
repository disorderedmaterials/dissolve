// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/overviewTab.h"
#include "gui/gui.h"
#include <QQuickWidget>
#include <QQmlContext>
#include <QQuickItem>

OverviewTab::OverviewTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title) : MainTab(dissolveWindow, dissolve, parent, title, this), dissolveModel_(dissolve)
{
    qmlRegisterType<DissolveModel>("Dissolve", 1, 0, "DissolveModel");
    view_ = new QQuickWidget(QUrl("qrc:/dialogs/qml/OverviewTab.qml"), this);
    view_->rootContext()->setContextProperty("dissolveModel", QVariant::fromValue(&dissolveModel_));
    view_->setMinimumSize(300, 300);
    view_->setResizeMode(QQuickWidget::SizeRootObjectToView);
}

MainTab::TabType OverviewTab::type() const
{
    return MainTab::TabType::Overview;
}

void OverviewTab::updateControls()
{
    dissolveModel_.reset();
    Messenger::print("There are {} rows and {} cols", dissolveModel_.rowCount(), dissolveModel_.columnCount());
    connect(view_->rootObject(), SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));
}

void OverviewTab::preventEditing()
{

}

void OverviewTab::allowEditing()
{

}

void OverviewTab::clicked(QModelIndex index) {
    Messenger::print("{} {}", index.row(), index.column());
}