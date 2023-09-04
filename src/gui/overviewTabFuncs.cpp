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
    qmlRegisterType<DissolveModel>("Dissolve", 1, 0, "DissolveModel");
    qmlRegisterType<SpeciesModel>("Dissolve", 1, 0, "SpeciesModel");
    qmlRegisterType<ConfigurationModel>("Dissolve", 1, 0, "ConfigurationModel");

    // Set up the model
    dissolveModel_.setDissolve(dissolve);

    // Create a layout
    QHBoxLayout *topLeftLayout = new QHBoxLayout(this);

    // Create the view
    view_ = new QQuickWidget(this);
    view_->rootContext()->setContextProperty("dissolveModel", &dissolveModel_);
    view_->setSource(QUrl("qrc:/overview/qml/overview/OverviewTab.qml"));
    view_->setMinimumSize(300, 300);

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
void OverviewTab::updateControls() { dissolveModel_.update(); }

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
        connect(view_->rootObject(), SIGNAL(atomTypesClicked()), this, SLOT(atomTypesClicked()));
        connect(view_->rootObject(), SIGNAL(masterTermsClicked()), this, SLOT(masterTermsClicked()));
        connect(view_->rootObject(), SIGNAL(configurationClicked(int)), this, SLOT(configurationClicked(int)));
        connect(view_->rootObject(), SIGNAL(speciesClicked(int)), this, SLOT(speciesClicked(int)));
        slotsAreSetup_ = true;
    }
}

// Atom types clicked
void OverviewTab::atomTypesClicked()
{
    tabWidget_->setCurrentIndex(1);
    auto *tab = dynamic_cast<ForcefieldTab *>(tabWidget_->currentTab());
    tab->setTab(0);
}
// Master terms clicked
void OverviewTab::masterTermsClicked()
{
    tabWidget_->setCurrentIndex(1);
    auto *tab = dynamic_cast<ForcefieldTab *>(tabWidget_->currentTab());
    tab->setTab(1);
}
// Species clicked
void OverviewTab::speciesClicked(int index)
{
    auto *species = dissolveModel_.speciesModel()
                        ->data(dissolveModel_.speciesModel()->index(index, 0), Qt::UserRole)
                        .value<const Species *>();
    tabWidget_->setCurrentTab(species);
}

// Configuration clicked
void OverviewTab::configurationClicked(int index)
{
    auto *configuration = dissolveModel_.configurationsModel()
                              ->data(dissolveModel_.configurationsModel()->index(index, 0), Qt::UserRole)
                              .value<Configuration *>();
    tabWidget_->setCurrentTab(configuration);
}