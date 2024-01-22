// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataManagerDialog.h"
#include "main/dissolve.h"
#include "templates/variantPointer.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>

DataManagerDialog::DataManagerDialog(QWidget *parent, Dissolve &dissolve, GenericList &items)
    : QDialog(parent), dissolve_(dissolve), simModel_(dissolve, items)
{
    ui_.setupUi(this);

    simProxy_.setSourceModel(&simModel_);
    ui_.SimulationDataTable->setModel(&simProxy_);
    ui_.SimulationDataTable->setSortingEnabled(true);

    updateControls();
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

// Update controls
void DataManagerDialog::updateControls()
{
    // Clear and re-populate simulation data table
    ui_.SimulationDataTable->resizeColumnsToContents();
}

// Simulation Data
void DataManagerDialog::on_SimulationDataFilterEdit_textChanged(const QString &text) { filterTable(text); }

// Dialog
void DataManagerDialog::on_CloseButton_clicked(bool checked) { accept(); }
