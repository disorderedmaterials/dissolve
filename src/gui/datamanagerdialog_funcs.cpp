/*
    *** Data Manager Dialog
    *** src/gui/datamanagerdialog_funcs.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "genericitems/item.h"
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

// Append GenericItems to table under specified source
void DataManagerDialog::addItemsToTable(QTableWidget *table, List<GenericItem> &items, const QString locationName,
                                        const QString locationIconResource)
{
    // Create icon
    QIcon locationIcon = QPixmap(locationIconResource);

    QTableWidgetItem *item;
    auto count = table->rowCount();
    table->setRowCount(count + items.nItems());
    ListIterator<GenericItem> itemIterator(items);
    while (GenericItem *genericItem = itemIterator.iterate())
    {
        // Item name
        item = new QTableWidgetItem(QString::fromStdString(std::string(genericItem->name())));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setData(Qt::UserRole, VariantPointer<GenericItem>(genericItem));
        table->setItem(count, 0, item);

        // Item type
        item = new QTableWidgetItem(QString::fromStdString(std::string(genericItem->itemClassName())));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(count, 1, item);

        // Version
        item = new QTableWidgetItem(QString::number(genericItem->version()));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(count, 2, item);

        // Location
        item = new QTableWidgetItem(locationName);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setIcon(locationIcon);
        table->setItem(count, 3, item);

        ++count;
    }
}

// Update the specified table of GenericItems, optionally filtering them by name and description
void DataManagerDialog::filterTable(QTableWidget *table, GenericItem *current, QString filter)
{
    // Loop over rows in the table
    for (int n = 0; n < table->rowCount(); ++n)
    {
        QTableWidgetItem *item = table->item(n, 0);
        if (!item)
            continue;
        GenericItem *genericItem = VariantPointer<GenericItem>(item->data(Qt::UserRole));
        if (genericItem == current)
        {
            table->setCurrentItem(item);
        }

        // Check filtering
        if (filter.isEmpty())
            table->setRowHidden(n, false);
        else
        {
            // Check name
            QString name = QString::fromStdString(std::string(genericItem->name()));
            auto inName = name.contains(QRegExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard));

            // Hide the item?
            auto hide = (!inName);
            table->setRowHidden(n, hide);

            // If the item was hidden, and it was selected, reset the current index
            if (hide && item->isSelected())
            {
                table->setCurrentItem(nullptr);
            }
        }
    }
}

// Update ReferencePoint table row
void DataManagerDialog::referencePointRowUpdate(int row, ReferencePoint *refPoint, bool createItems)
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
    addItemsToTable(ui_.SimulationDataTable, dissolve_.processingModuleData().items(), "Main Processing",
                    ":/dissolve/icons/dissolve.png");
    ListIterator<Configuration> configIterator(dissolve_.configurations());
    while (Configuration *cfg = configIterator.iterate())
        addItemsToTable(ui_.SimulationDataTable, cfg->moduleData().items(), QString::fromStdString(std::string(cfg->name())),
                        ":/tabs/icons/tabs_configuration.svg");
    ui_.SimulationDataTable->resizeColumnsToContents();

    // Populate reference points table
    TableWidgetUpdater<DataManagerDialog, ReferencePoint> refPointUpdater(ui_.ReferencePointsTable, referencePoints_, this,
                                                                          &DataManagerDialog::referencePointRowUpdate);
    ui_.ReferencePointsTable->resizeColumnsToContents();
}

// Simulation Data
void DataManagerDialog::on_SimulationDataFilterEdit_textChanged(const QString &text)
{
    filterTable(ui_.SimulationDataTable, nullptr, text);
}

// Refernce Points
void DataManagerDialog::on_ReferencePointRemoveButton_clicked(bool checked)
{
    ReferencePoint *refPoint = currentReferencePoint();
    if (!refPoint)
        return;

    // For the provided suffix, we need to prune all processing data lists of associated data
    dissolve_.processingModuleData().pruneWithSuffix(refPoint->suffix());
    ListIterator<Configuration> configIterator(dissolve_.configurations());
    while (Configuration *cfg = configIterator.iterate())
        cfg->moduleData().pruneWithSuffix(refPoint->suffix());

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
