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

DataManagerDialog::DataManagerDialog(QWidget *parent, Dissolve &dissolve, std::vector<ReferencePoint> &referencePoints)
    : QDialog(parent), dissolve_(dissolve), referencePoints_(referencePoints), refModel_(referencePoints_)
{
    ui_.setupUi(this);
    ui_.ReferencePointsTable->setModel(&refModel_);

    updateControls();
}

DataManagerDialog::~DataManagerDialog() {}

/*
 * UI
 */

// Add GenericItems to table
void DataManagerDialog::addItems(const std::map<std::string, GenericItem::Type> &items)
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
        item = new QTableWidgetItem(QString::fromStdString(std::string(std::get<GenericItem::ClassName>(value))));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui_.SimulationDataTable->setItem(count, 1, item);

        // Version
        item = new QTableWidgetItem(QString::number(std::get<GenericItem::Version>(value)));
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

// Return currently-selected ReferencePoint
ReferencePoint *DataManagerDialog::currentReferencePoint() const
{
    // Get current item from tree, and check the parent item
    auto index = ui_.ReferencePointsTable->currentIndex();
    if (!index.isValid())
        return nullptr;
    return VariantPointer<ReferencePoint>(refModel_.data(index, Qt::UserRole));
}

// Update controls
void DataManagerDialog::updateControls()
{
    // Clear and re-populate simulation data table
    ui_.SimulationDataTable->setRowCount(0);
    addItems(dissolve_.processingModuleData().items());
    ui_.SimulationDataTable->resizeColumnsToContents();

    // Populate reference points table
    refModel_.update();
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

    referencePoints_.emplace_back(qPrintable(suffix), qPrintable(QDir::current().relativeFilePath(restartFile)));
    auto &refPoint = referencePoints_.back();

    // Load the data
    if (!dissolve_.loadRestartAsReference(refPoint.restartFile(), refPoint.suffix()))
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

    referencePoints_.emplace_back(qPrintable(suffix), qPrintable(QDir::current().relativeFilePath(filename)));

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
