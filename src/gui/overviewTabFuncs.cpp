// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/overviewTab.h"
#include "gui/gui.h"
#include <QQuickWidget>
#include <QQmlContext>
#include <QQuickItem>

OverviewTab::OverviewTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title) : MainTab(dissolveWindow, dissolve, parent, title, this), dissolveModel_(dissolve)
{
    QHBoxLayout *topLeftLayout = new QHBoxLayout(this);
    qmlRegisterType<DissolveModel>("Dissolve", 1, 0, "DissolveModel");
    view_ = new QQuickWidget(this);
    view_->rootContext()->setContextProperty("dissolveModel", QVariant::fromValue(&dissolveModel_));
    view_->setSource(QUrl("qrc:/dialogs/qml/OverviewTab.qml"));
    connect(view_, SIGNAL(statusChanged(QQuickWidget::Status)), SLOT(viewStatusChanged()));

    topLeftLayout->addWidget(view_);
    topLeftLayout->setAlignment(view_, Qt::AlignHCenter);
}

MainTab::TabType OverviewTab::type() const
{
    return MainTab::TabType::Overview;
}

void OverviewTab::updateControls()
{
    dissolveModel_.reset();
}

void OverviewTab::preventEditing() {}

void OverviewTab::allowEditing() {}

void OverviewTab::viewStatusChanged()
{
    if (view_->status() == QQuickWidget::Status::Ready && !slotsAreSetup_)
    {
        connect(view_->rootObject(), SIGNAL(clicked(int, int)), this, SLOT(clicked(int, int)));
        slotsAreSetup_ = true;
    }
}

void OverviewTab::clicked(int row, int col) {
    auto obj = dissolveModel_.rawData(dissolveModel_.index(row, col));
    auto* sp = obj.value<Species*>();
    auto* cfg = obj.value<Configuration*>();
    if (sp)
        tabWidget_->setCurrentTab(sp);
    else if (cfg)
        tabWidget_->setCurrentTab(cfg);
    else
    {
        tabWidget_->setCurrentIndex(1);
        auto* tab = dynamic_cast<ForcefieldTab*>(tabWidget_->currentTab());
        tab->setTabIndex(std::min(col, 1));
    }
}