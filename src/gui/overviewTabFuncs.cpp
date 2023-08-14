// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/overviewTab.h"
#include "gui/gui.h"
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>

OverviewTab::OverviewTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title) : MainTab(dissolveWindow, dissolve, parent, title, this), dissolveModel_(dissolve)
{
    qmlRegisterType<DissolveModel>("Dissolve", 1, 0, "DissolveModel");
    QQuickWidget *view = new QQuickWidget(QUrl("qrc:/dialogs/qml/OverviewTab.qml"), this);
    //view->rootContext()->setContextProperty("dissolveModel", QVariant::fromValue(&dissolveModel_));

    auto* root = view->rootObject();
    connect(view->rootObject(), SIGNAL(clickedy()), this, SLOT(clicky()));
}

MainTab::TabType OverviewTab::type() const
{
    return MainTab::TabType::Overview;
}

void OverviewTab::updateControls()
{
    dissolveModel_.reset();
    Messenger::print("There are {} rows and {} cols", dissolveModel_.rowCount(), dissolveModel_.columnCount());
}

void OverviewTab::preventEditing()
{

}

void OverviewTab::allowEditing()
{

}

void OverviewTab::clicky() {Messenger::print("got it\n"); }