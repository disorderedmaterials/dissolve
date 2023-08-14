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
    view_ = new QQuickWidget(this);
    view_->rootContext()->setContextProperty("dissolveModel", QVariant::fromValue(&dissolveModel_));
    view_->setSource(QUrl("qrc:/dialogs/qml/OverviewTab.qml"));
    view_->setMinimumSize(300, 300);
    view_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    connect(view_, SIGNAL(statusChanged(QQuickWidget::Status)), SLOT(viewStatusChanged()));
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

void OverviewTab::viewStatusChanged()
{
    if (view_->status() == QQuickWidget::Status::Ready && !slotsAreSetup_)
    {
        connect(view_->rootObject(), SIGNAL(clicked(int, int)), this, SLOT(clicked(int, int)));
    }
}

void OverviewTab::clicked(int row, int col) {
    Messenger::print("{} {}", row, col);
    auto i = dissolveModel_.index(row, col);
    QVariant obj = dissolveModel_.rawData(dissolveModel_.index(row, col));

    auto* sp = obj.value<Species*>();
    if (sp)
    {
        return Messenger::print("{}", sp->name());
    }

    auto *cfg = obj.value<Configuration*>();
    if (cfg)
    {
        return Messenger::print("{}", cfg->name());
    }

}