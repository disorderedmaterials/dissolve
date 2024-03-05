// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataManagerDialog.h"
#include "main/dissolve.h"
#include "templates/variantPointer.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QQmlContext>
#include <QRegularExpression>

DataManagerDialog::DataManagerDialog(QWidget *parent, Dissolve &dissolve, GenericList &items)
    : QDialog(parent), dissolve_(dissolve), simModel_(dissolve, items)
{
    view_ = new QQuickWidget(this);
    simProxy_.setSourceModel(&simModel_);
    view_->rootContext()->setContextProperty("simProxy", &simProxy_);
    view_->rootContext()->setContextProperty("simModel", &simModel_);
    view_->setSource(QUrl("qrc:/dialogs/qml/simulationDataManager/SimulationDataManager.qml"));

    view_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    auto *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(view_);
    setLayout(topLeftLayout);

    QObject::connect(&simModel_, SIGNAL(closeClicked()), this, SLOT(accept()));
}