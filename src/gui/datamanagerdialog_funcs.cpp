// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/datamanagerdialog.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegExp>

DataManagerDialog::DataManagerDialog(QWidget *parent, Dissolve &dissolve, List<ReferencePoint> &referencePoints)
    : QDialog(parent), dissolve_(dissolve), referencePoints_(referencePoints)
{
    ui_.setupUi(this);

    updateControls();
}

DataManagerDialog::~DataManagerDialog() {}

/*
 * UI
 */

// Add GenericItems to table
void DataManagerDialog::addItems(const std::map<std::string, GenericList::GenericItem> &items)
{
    QTableWidgetItem *item;
    ui_.SimulationDataTable->setRowCount(items.size());
    auto count = 0;
    for (auto &[key, value] : items)
    {
        // Item name
        item = new QTableWidgetItem(QString::fromStdString(std::string(key)));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui_.SimulationDataTable->setItem(count, 0, item);

        // Item type
        item = new QTableWidgetItem(QString::fromStdString(std::string(std::get<GenericList::ItemData::ClassName>(value))));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui_.SimulationDataTable->setItem(count, 1, item);

        // Version
        item = new QTableWidgetItem(QString::number(std::get<GenericList::ItemData::Version>(value)));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui_.SimulationDataTable->setItem(count, 2, item);

        ++count;
    }
}

// Update the specified table of GenericItems, optionally filtering them by name and description
void DataManagerDialog::filterTable(QString filterText)
{
    // Loop over rows in the table
    for (auto n = 0; n < ui_.SimulationDataTable->rowCount(); ++n)
    {
        QTableWidgetItem *item = ui_.SimulationDataTable->item(n, 0);
        if (!item)
            continue;

        // Check filtering
        if (filterText.isEmpty() || !item->text().contains(QRegExp(filterText, Qt::CaseInsensitive, QRegExp::Wildcard)))
            ui_.SimulationDataTable->setRowHidden(n, false);
        else
        {
            if (item->isSelected())
                ui_.SimulationDataTable->setCurrentItem(nullptr);

            ui_.SimulationDataTable->setRowHidden(n, true);
        }
    }
}

// Update ReferencePoint table row
void DataManagerDialog::referencePointRowUpdate(int row, const ReferencePoint *refPoint, bool createItems)
{
    QTableWidgetItem *item;

    // Data Suffix
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<ReferencePoint>(refPoint));
        ui_.ReferencePointsTable->setItem(row, 0, item);
    }
    else
        item = ui_.ReferencePointsTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(refPoint->suffix())));

    // Restart file name
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<ReferencePoint>(refPoint));
        ui_.ReferencePointsTable->setItem(row, 1, item);
    }
    else
        item = ui_.ReferencePointsTable->item(row, 1);
    item->setText(QString::fromStdString(std::string(refPoint->restartFile())));
}

// Return currently-selected ReferencePoint
ReferencePoint *DataManagerDialog::currentReferencePoint() const
{
    // Get current item from tree, and check the parent item
    QTableWidgetItem *item = ui_.ReferencePointsTable->currentItem();
    if (!item)
        return nullptr;
    return VariantPointer<ReferencePoint>(item->data(Qt::UserRole));
}

// Update controls
void DataManagerDialog::updateControls()
{
    // Clear and re-populate simulation data table
    ui_.SimulationDataTable->setRowCount(0);
    addItems(dissolve_.processingModuleData().items());
    ui_.SimulationDataTable->resizeColumnsToContents();

    // Populate reference points table
    ConstTableWidgetUpdater<DataManagerDialog, ReferencePoint> refPointUpdater(ui_.ReferencePointsTable, referencePoints_, this,
                                                                               &DataManagerDialog::referencePointRowUpdate);
    ui_.ReferencePointsTable->resizeColumnsToContents();
}

// Simulation Data
void DataManagerDialog::on_SimulationDataFilterEdit_textChanged(const QString &text) { filterTable(text); }

// Refernce Points
void DataManagerDialog::on_ReferencePointRemoveButton_clicked(bool checked)
{
    ReferencePoint *refPoint = currentReferencePoint();
    if (!refPoint)
        return;

    dissolve_.processingModuleData().pruneWithSuffix(refPoint->suffix());

    updateControls();
}

void DataManagerDialog::on_ReferencePointOpenButton_clicked(bool checked)
{
    // Get file to open
    QString restartFile =
        QFileDialog::getOpenFileName(this, "Choose restart file to open", QDir().absolutePath(), "Restart Files (*.restart)");
    if (restartFile.isEmpty())
        return;

    // Get suffix for data - base this on the filename
    QString suffix = QFileInfo(restartFile).baseName();
    bool ok;
    suffix = QInputDialog::getText(this, "Reference Point Suffix", "Enter suffix to apply to the reference point data",
                                   QLineEdit::Normal, suffix, &ok);
    if (!ok)
        return;

    ReferencePoint *refPoint = referencePoints_.add();
    refPoint->setRestartFile(qPrintable(QDir::current().relativeFilePath(restartFile)));
    refPoint->setSuffix(qPrintable(suffix));

    // Load the data
    if (!dissolve_.loadRestartAsReference(refPoint->restartFile(), refPoint->suffix()))
        QMessageBox::warning(this, "Error loading reference point",
                             "Couldn't load the reference point data specified.\nThis may be because your simulation "
                             "setup doesn't match that expected by the restart data.\n");

    updateControls();
}

void DataManagerDialog::on_ReferencePointCreateButton_clicked(bool checked)
{
    // Get filename for reference point
    QString filename =
        QFileDialog::getSaveFileName(this, "Select Reference Point File", QDir::currentPath(), "Restart Files (*.restart)");
    if (filename.isEmpty())
        return;

    // Get suffix for data - base this on the filename
    QString suffix = QFileInfo(filename).baseName();
    bool ok;
    suffix = QInputDialog::getText(this, "Reference Point Suffix", "Enter suffix to apply to the reference point data",
                                   QLineEdit::Normal, suffix, &ok);
    if (!ok)
        return;

    if (dissolve_.saveRestart(qPrintable(filename)))
        Messenger::print("Saved reference point to '{}'.\n", qPrintable(filename));
    else
    {
        Messenger::error("Failed to save reference point to '{}'.\n", qPrintable(filename));
        return;
    }

    ReferencePoint *refPoint = referencePoints_.add();
    refPoint->setRestartFile(qPrintable(QDir::current().relativeFilePath(filename)));
    refPoint->setSuffix(qPrintable(suffix));

    if (!dissolve_.loadRestartAsReference(qPrintable(filename), qPrintable(suffix)))
        QMessageBox::warning(this, "Error loading reference point",
                             "Couldn't load the reference point data.\nWhich is odd, annoying, and something you "
                             "should let the developer know about.");

    updateControls();
}

void DataManagerDialog::on_ReferencePointsTable_currentItemChanged(QTableWidgetItem *currentItem,
                                                                   QTableWidgetItem *previousItem)
{
    ui_.ReferencePointRemoveButton->setEnabled(currentItem);
}

// Dialog
void DataManagerDialog::on_CloseButton_clicked(bool checked) { accept(); }
