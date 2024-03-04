// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataManagerDialog.h"
#include "main/dissolve.h"
#include "templates/variantPointer.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QQmlContext>

DataManagerDialog::DataManagerDialog(QWidget *parent, Dissolve &dissolve, GenericList &items)
    : QDialog(parent), dissolve_(dissolve), simModel_(dissolve, items)
{
    view_ = new QQuickWidget(this);
    simProxy_.setSourceModel(&simModel_);
    view_->rootContext()->setContextProperty("simProxy", &simProxy_);
    view_->rootContext()->setContextProperty("simModel", &simModel_);
    view_->setSource(QUrl("qrc:/dialogs/qml/simulationDataManager/SimulationDataManager.qml"));

    QObject::connect(&simModel_, SIGNAL(closeClicked()), this, SLOT(accept()));
    QObject::connect(&simModel_, SIGNAL(simulationDataFilter()), this, SLOT(on_SimulationDataFilterEdit_textChanged()));
}

/*
 * UI
 */

// Update the specified table of GenericItems, optionally filtering them by name and description
void DataManagerDialog::filterTable(QString filterText)
{
    simProxy_.setFilterRegularExpression(
        QRegularExpression(filterText.replace("*", ".*"), QRegularExpression::CaseInsensitiveOption));
    simProxy_.setFilterKeyColumn(0);
}

// Simulation Data
void DataManagerDialog::on_SimulationDataFilterEdit_textChanged(const QString &text) { filterTable(text); }