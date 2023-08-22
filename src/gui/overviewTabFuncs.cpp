// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/overviewTab.h"
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>

OverviewTab::OverviewTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title)
    : MainTab(dissolveWindow, dissolve, parent, title, this)
{
    //dissolveModel_.setDissolve(dissolve);
    // Create a layout
    QHBoxLayout *topLeftLayout = new QHBoxLayout(this);


    //qmlRegisterType<AtomTypeModel>("Dissolve", 1, 0, "AtomTypeModel");
    // Create the view
    view_ = new QQuickWidget(this);
    dissolveModel_.setDissolve(dissolve);
    view_->rootContext()->setContextProperty("dissolveModel", QVariant::fromValue(&dissolveModel_));

    view_->setSource(QUrl("qrc:/dialogs/qml/OverviewTab.qml"));
    //view_->setMinimumSize(300, 300);
    //view_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    //model->setDissolve(dissolve_);
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
void OverviewTab::updateControls() { dissolveModel_.reset();     view_->rootContext()->setContextProperty("dissolveModel", QVariant::fromValue(&dissolveModel_));
}
    //auto model = view_->rootObject()->findChild<DissolveModel *>("dissolveModel");
    //model->setDissolve(dissolve_);
    //Messenger::print("{}",model->atomTypes()->rowCount());

 //dissolveModel_.reset(); }

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
        //dissolveModel_.setDissolve(dissolve_);
        //view_->rootContext()->setContextProperty("atModel", QVariant::fromValue(dissolveModel_.atomTypes()));
        //auto model = view_->rootObject()->findChild<AtomTypeModel *>("atModel");
        //model->setData(dissolve_.coreData().atomTypes());;
        //view_->rootContext()->setContextProperty("dissolveModel", QVariant::fromValue(&dissolveModel_));
        // If the view is ready, then connect our slots to its signals
        //connect(view_->rootObject(), SIGNAL(nodeClicked(int, int)), this, SLOT(nodeClicked(int, int)));
        //auto root = view_->rootObject();
        //auto model = root->findChild<DissolveModel *>("dissolveModel");
        //model->setDissolve(dissolve_);
        slotsAreSetup_ = true;
    }
}

// Node has been clicked
void OverviewTab::nodeClicked(int row, int col)
{
    /*auto obj = dissolveModel_.rawData(dissolveModel_.index(row, col));
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
        tab->setTab(std::min(col, 1));
    }*/
}