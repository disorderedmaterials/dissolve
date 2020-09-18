/*
    *** SpeciesTab Functions - Geometry
    *** src/gui/speciestab_geometry.cpp
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

#include "classes/atomtype.h"
#include "gui/gui.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/selectforcefielddialog.h"
#include "gui/speciestab.h"
#include "main/dissolve.h"
#include <QMessageBox>

/*
 * Private Functions
 */

// SpeciesAtomTable row update function
void SpeciesTab::updateAtomTableRow(int row, SpeciesAtom *speciesAtom, bool createItems)
{
    QTableWidgetItem *item;

    // Element
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui_.AtomTable->setItem(row, 0, item);
    }
    else
        item = ui_.AtomTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(speciesAtom->element()->name())));
    item->setSelected(speciesAtom->isSelected());

    // AtomType
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
        ui_.AtomTable->setItem(row, 1, item);
    }
    else
        item = ui_.AtomTable->item(row, 1);
    item->setText(speciesAtom->atomType() ? QString::fromStdString(std::string(speciesAtom->atomType()->name())) : "");
    item->setSelected(speciesAtom->isSelected());

    // Coordinates
    for (auto n = 0; n < 3; ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
            ui_.AtomTable->setItem(row, n + 2, item);
        }
        else
            item = ui_.AtomTable->item(row, n + 2);
        item->setText(QString::number(speciesAtom->r().get(n)));
        item->setSelected(speciesAtom->isSelected());
    }

    // Charge
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
        ui_.AtomTable->setItem(row, 5, item);
    }
    else
        item = ui_.AtomTable->item(row, 5);
    item->setText(QString::number(speciesAtom->charge()));
    item->setSelected(speciesAtom->isSelected());
    item->setFlags(dissolve_.pairPotentialsIncludeCoulomb() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable
                                                            : Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
}

// BondTable row update function
void SpeciesTab::updateBondTableRow(int row, SpeciesBond *speciesBond, bool createItems)
{
    QTableWidgetItem *item;

    // Atom Indices
    for (auto n = 0; n < 2; ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui_.BondTable->setItem(row, n, item);
        }
        else
            item = ui_.BondTable->item(row, n);

        item->setText(QString::number(speciesBond->index(n) + 1));
    }

    // Interaction Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
        ui_.BondTable->setItem(row, 2, item);
    }
    else
        item = ui_.BondTable->item(row, 2);
    item->setText(speciesBond->masterParameters()
                      ? QString("@%1").arg(QString::fromStdString(std::string(speciesBond->masterParameters()->name())))
                      : QString::fromStdString(std::string(SpeciesBond::bondFunctions().keywordFromInt(speciesBond->form()))));

    // Interaction Parameters
    for (auto n = 0; n < speciesBond->nParameters(); ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
            ui_.BondTable->setItem(row, n + 3, item);
        }
        else
            item = ui_.BondTable->item(row, n + 3);

        item->setText(QString::number(speciesBond->parameter(n)));
        item->setFlags(speciesBond->masterParameters() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable
                                                       : Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }
}

// AngleTable row update function
void SpeciesTab::updateAngleTableRow(int row, SpeciesAngle *speciesAngle, bool createItems)
{
    QTableWidgetItem *item;

    // Atom Indices
    for (auto n = 0; n < 3; ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui_.AngleTable->setItem(row, n, item);
        }
        else
            item = ui_.AngleTable->item(row, n);

        item->setText(QString::number(speciesAngle->index(n) + 1));
    }

    // Interaction Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
        ui_.AngleTable->setItem(row, 3, item);
    }
    else
        item = ui_.AngleTable->item(row, 3);

    item->setText(
        speciesAngle->masterParameters()
            ? QString("@%1").arg(QString::fromStdString(std::string(speciesAngle->masterParameters()->name())))
            : QString::fromStdString(std::string(SpeciesAngle::angleFunctions().keywordFromInt(speciesAngle->form()))));

    // Interaction Parameters
    for (auto n = 0; n < speciesAngle->nParameters(); ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
            ui_.AngleTable->setItem(row, n + 4, item);
        }
        else
            item = ui_.AngleTable->item(row, n + 4);

        item->setText(QString::number(speciesAngle->parameter(n)));
        item->setFlags(speciesAngle->masterParameters() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable
                                                        : Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }
}

// TorsionTable row update function
void SpeciesTab::updateTorsionTableRow(int row, SpeciesTorsion *speciesTorsion, bool createItems)
{
    QTableWidgetItem *item;

    // Atom Indices
    for (auto n = 0; n < 4; ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui_.TorsionTable->setItem(row, n, item);
        }
        else
            item = ui_.TorsionTable->item(row, n);

        item->setText(QString::number(speciesTorsion->index(n) + 1));
    }

    // Interaction Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
        ui_.TorsionTable->setItem(row, 4, item);
    }
    else
        item = ui_.TorsionTable->item(row, 4);
    item->setText(
        speciesTorsion->masterParameters()
            ? QString("@%1").arg(QString::fromStdString(std::string(speciesTorsion->masterParameters()->name())))
            : QString::fromStdString(std::string(SpeciesTorsion::torsionFunctions().keywordFromInt(speciesTorsion->form()))));

    // Interaction Parameters
    for (auto n = 0; n < speciesTorsion->nParameters(); ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui_.TorsionTable->setItem(row, n + 5, item);
        }
        else
            item = ui_.TorsionTable->item(row, n + 5);

        item->setText(QString::number(speciesTorsion->parameter(n)));
    }
}

// ImproperTable row update function
void SpeciesTab::updateImproperTableRow(int row, SpeciesImproper *speciesImproper, bool createItems)
{
    QTableWidgetItem *item;

    // Atom Indices
    for (auto n = 0; n < 4; ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesImproper>(speciesImproper));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui_.ImproperTable->setItem(row, n, item);
        }
        else
            item = ui_.ImproperTable->item(row, n);
        item->setText(QString::number(speciesImproper->index(n) + 1));
    }

    // Interaction Form
    if (createItems)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<SpeciesImproper>(speciesImproper));
        ui_.ImproperTable->setItem(row, 4, item);
    }
    else
        item = ui_.ImproperTable->item(row, 4);

    item->setText(speciesImproper->masterParameters()
                      ? QString("@%1").arg(QString::fromStdString(std::string(speciesImproper->masterParameters()->name())))
                      : QString::fromStdString(
                            std::string(SpeciesImproper::improperFunctions().keywordFromInt(speciesImproper->form()))));

    // Interaction Parameters
    for (auto n = 0; n < speciesImproper->nParameters(); ++n)
    {
        if (createItems)
        {
            item = new QTableWidgetItem;
            item->setData(Qt::UserRole, VariantPointer<SpeciesImproper>(speciesImproper));
            ui_.ImproperTable->setItem(row, n + 5, item);
        }
        else
            item = ui_.ImproperTable->item(row, n + 5);

        item->setText(QString::number(speciesImproper->parameter(n)));
        item->setFlags(speciesImproper->masterParameters() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable
                                                           : Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }
}

/*
 * Private Slots
 */

// Update atom table selection
void SpeciesTab::updateAtomTableSelection()
{
    Locker refreshLocker(refreshLock_);

    QTableWidgetItem *item;
    SpeciesAtom *i;

    // Set atom selection in table to reflect the atom data
    for (auto n = 0; n < ui_.AtomTable->rowCount(); ++n)
    {
        item = ui_.AtomTable->item(n, 0);
        i = VariantPointer<SpeciesAtom>(item->data(Qt::UserRole));

        if (i->isSelected())
            for (int m = 0; m < 6; ++m)
                ui_.AtomTable->item(n, m)->setSelected(true);
        else
            for (int m = 0; m < 6; ++m)
                ui_.AtomTable->item(n, m)->setSelected(false);
    }
}

void SpeciesTab::on_AtomTable_itemChanged(QTableWidgetItem *w)
{
    std::optional<std::shared_ptr<AtomType>> opt_atomType;
    if (refreshLock_.isLocked())
        return;

    // Get target SpeciesAtom from the passed widget
    SpeciesAtom *speciesAtom = w ? VariantPointer<SpeciesAtom>(w->data(Qt::UserRole)) : nullptr;
    if (!speciesAtom)
        return;
    Vec3<double> r = speciesAtom->r();
    // Column of passed item tells us the type of data we need to change
    std::shared_ptr<AtomType> atomType;
    switch (w->column())
    {
        // Element
        case (0):
            break;
        // AtomType
        case (1):
            // Check the text to see if it is an existing AtomType - if not, we should create it
            opt_atomType = dissolve_.findAtomType(qPrintable(w->text()));
            if (!opt_atomType)
            {
                atomType = dissolve_.addAtomType(speciesAtom->element());
                atomType->setName(qPrintable(w->text()));
            }
            else
                atomType = *opt_atomType;
            speciesAtom->setAtomType(atomType);
            dissolveWindow_->setModified();
            break;
        // Coordinates
        case (2):
        case (3):
        case (4):
            switch (w->column())
            {
                case 2:
                    r.x = w->text().toDouble();
                    break;
                case 3:
                    r.y = w->text().toDouble();
                    break;
                case 4:
                    r.z = w->text().toDouble();
                    break;
            }
            species_->setAtomCoordinates(speciesAtom, r);
            dissolveWindow_->setModified();
            break;
        // Charge
        case (5):
            speciesAtom->setCharge(w->text().toDouble());
            // TODO This change needs to be propagated to all Configurations->Molecules based on this Species
            dissolveWindow_->setModified();
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of SpeciesAtom table.\n", w->column());
            break;
    }
}

void SpeciesTab::on_AtomTable_itemSelectionChanged()
{
    if (refreshLock_.isLocked())
        return;

    QTableWidgetItem *item;
    SpeciesAtom *i;

    // Set atom selection in viewer to be same as the table
    for (auto n = 0; n < ui_.AtomTable->rowCount(); ++n)
    {
        item = ui_.AtomTable->item(n, 0);
        i = VariantPointer<SpeciesAtom>(item->data(Qt::UserRole));

        if (item->isSelected())
            species_->selectAtom(i);
        else
            species_->deselectAtom(i);
    }

    // Recreate selection primitive and update viewer
    ui_.ViewerWidget->speciesViewer()->recreateSelectionPrimitive();
    ui_.ViewerWidget->updateStatusBar();
}

void SpeciesTab::on_BondTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked() || (!species_))
        return;

    // Get target SpeciesBond from the passed widget
    SpeciesBond *speciesBond = w ? VariantPointer<SpeciesBond>(w->data(Qt::UserRole)) : nullptr;
    if (!speciesBond)
        return;

    // Column of passed item tells us the type of data we need to change
    auto updateRow = false;
    switch (w->column())
    {
        // Atom Indices
        case (0):
        case (1):
            Messenger::error("Atom indices in intramolecular interactions are not editable.\n");
            break;
        // Functional Form
        case (2):
            // If the text starts with an '@' then its a reference to a master term
            if (w->text().at(0) == '@')
            {
                MasterIntra *master = dissolve_.coreData().hasMasterBond(qPrintable(w->text()));
                speciesBond->setMasterParameters(master);
            }
            else
            {
                SpeciesBond::BondFunction bf = SpeciesBond::bondFunctions().enumeration(qPrintable(w->text()));
                speciesBond->setMasterParameters(nullptr);
                speciesBond->setForm(bf);
            }
            updateRow = true;
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (3):
        case (4):
        case (5):
        case (6):
            if (speciesBond->masterParameters())
                Messenger::error("Refusing to set bond parameter since it belongs to a master term.\n");
            else
                speciesBond->setParameter(w->column() - 3, w->text().toDouble());
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of Bond table.\n", w->column());
            break;
    }

    // Update the row if necessary
    if (updateRow)
    {
        Locker refreshLocker(refreshLock_);
        updateBondTableRow(w->row(), speciesBond, false);
    }
}

void SpeciesTab::on_AngleTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked() || (!species_))
        return;

    // Get target SpeciesAngle from the passed widget
    SpeciesAngle *speciesAngle = w ? VariantPointer<SpeciesAngle>(w->data(Qt::UserRole)) : nullptr;
    if (!speciesAngle)
        return;

    // Column of passed item tells us the type of data we need to change
    auto updateRow = false;
    switch (w->column())
    {
        // Atom Indices
        case (0):
        case (1):
        case (2):
            Messenger::error("Atom indices in intramolecular interactions are not editable.\n");
            break;
        // Functional Form
        case (3):
            // If the text starts with an '@' then its a reference to a master term
            if (w->text().at(0) == '@')
            {
                MasterIntra *master = dissolve_.coreData().hasMasterAngle(qPrintable(w->text()));
                speciesAngle->setMasterParameters(master);
            }
            else
            {
                SpeciesAngle::AngleFunction af = SpeciesAngle::angleFunctions().enumeration(qPrintable(w->text()));
                speciesAngle->setMasterParameters(nullptr);
                speciesAngle->setForm(af);
            }
            updateRow = true;
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (4):
        case (5):
        case (6):
        case (7):
            if (speciesAngle->masterParameters())
                Messenger::error("Refusing to set angle parameter since it belongs to a master term.\n");
            else
                speciesAngle->setParameter(w->column() - 4, w->text().toDouble());
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of Angle table.\n", w->column());
            break;
    }

    // Update the row if necessary
    if (updateRow)
    {
        Locker refreshLocker(refreshLock_);
        updateAngleTableRow(w->row(), speciesAngle, false);
    }
}

void SpeciesTab::on_TorsionTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked() || (!species_))
        return;

    // Get target SpeciesTorsion from the passed widget
    SpeciesTorsion *speciesTorsion = w ? VariantPointer<SpeciesTorsion>(w->data(Qt::UserRole)) : nullptr;
    if (!speciesTorsion)
        return;

    // Column of passed item tells us the type of data we need to change
    auto updateRow = false;
    switch (w->column())
    {
        // Atom Indices
        case (0):
        case (1):
        case (2):
        case (3):
            Messenger::error("Atom indices in intramolecular interactions are not editable.\n");
            break;
        // Functional Form
        case (4):
            // If the text starts with an '@' then its a reference to a master term
            if (w->text().at(0) == '@')
            {
                MasterIntra *master = dissolve_.coreData().hasMasterTorsion(qPrintable(w->text()));
                speciesTorsion->setMasterParameters(master);
            }
            else
            {
                SpeciesTorsion::TorsionFunction tf = SpeciesTorsion::torsionFunctions().enumeration(qPrintable(w->text()));
                speciesTorsion->setMasterParameters(nullptr);
                speciesTorsion->setForm(tf);
            }
            updateRow = true;
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (5):
        case (6):
        case (7):
        case (8):
            if (speciesTorsion->masterParameters())
                Messenger::error("Refusing to set torsion parameter since it belongs to a master term.\n");
            else
                speciesTorsion->setParameter(w->column() - 5, w->text().toDouble());
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of Torsion table.\n", w->column());
            break;
    }

    // Update the row if necessary
    if (updateRow)
    {
        Locker refreshLocker(refreshLock_);
        updateTorsionTableRow(w->row(), speciesTorsion, false);
    }
}

void SpeciesTab::on_ImproperTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshLock_.isLocked() || (!species_))
        return;

    // Get target SpeciesImproper from the passed widget
    SpeciesImproper *speciesImproper = w ? VariantPointer<SpeciesImproper>(w->data(Qt::UserRole)) : nullptr;
    if (!speciesImproper)
        return;

    // Column of passed item tells us the type of data we need to change
    auto updateRow = false;
    switch (w->column())
    {
        // Atom Indices
        case (0):
        case (1):
        case (2):
        case (3):
            Messenger::error("Atom indices in intramolecular interactions are not editable.\n");
            break;
        // Functional Form
        case (4):
            // If the text starts with an '@' then its a reference to a master term
            if (w->text().at(0) == '@')
            {
                MasterIntra *master = dissolve_.coreData().hasMasterImproper(qPrintable(w->text()));
                speciesImproper->setMasterParameters(master);
            }
            else
            {
                SpeciesImproper::ImproperFunction tf = SpeciesImproper::improperFunctions().enumeration(qPrintable(w->text()));
                speciesImproper->setMasterParameters(nullptr);
                speciesImproper->setForm(tf);
            }
            updateRow = true;
            dissolveWindow_->setModified();
            break;
        // Parameters
        case (5):
        case (6):
        case (7):
        case (8):
            if (speciesImproper->masterParameters())
                Messenger::error("Refusing to set improper parameter since it belongs to a master term.\n");
            else
                speciesImproper->setParameter(w->column() - 5, w->text().toDouble());
            break;
        default:
            Messenger::error("Don't know what to do with data from column {} of Improper table.\n", w->column());
            break;
    }

    // Update the row if necessary
    if (updateRow)
    {
        Locker refreshLocker(refreshLock_);
        updateImproperTableRow(w->row(), speciesImproper, false);
    }
}

/*
 * Public Functions
 */

// Update Geometry tab
void SpeciesTab::updateGeometryTab()
{
    Locker refreshLocker(refreshLock_);

    // -- SpeciesAtom Table
    // 	if (dissolve_.pairPotentialsIncludeCoulomb()) ui_.AtomTable->showColumn(5);
    // 	else ui_.AtomTable->hideColumn(5);
    if (!species_)
        ui_.AtomTable->clearContents();
    else
        TableWidgetUpdater<SpeciesTab, SpeciesAtom> speciesAtomUpdater(ui_.AtomTable, species_->atoms(), this,
                                                                       &SpeciesTab::updateAtomTableRow);

    // -- Intramolecular terms
    if (!species_)
    {
        ui_.BondTable->clearContents();
        ui_.AngleTable->clearContents();
        ui_.TorsionTable->clearContents();
        ui_.ImproperTable->clearContents();
    }
    else
    {
        TableWidgetUpdater<SpeciesTab, SpeciesBond> bondUpdater(ui_.BondTable, species_->bonds(), this,
                                                                &SpeciesTab::updateBondTableRow);
        TableWidgetUpdater<SpeciesTab, SpeciesAngle> angleUpdater(ui_.AngleTable, species_->angles(), this,
                                                                  &SpeciesTab::updateAngleTableRow);
        TableWidgetUpdater<SpeciesTab, SpeciesTorsion> torsionUpdater(ui_.TorsionTable, species_->torsions(), this,
                                                                      &SpeciesTab::updateTorsionTableRow);
        TableWidgetUpdater<SpeciesTab, SpeciesImproper> improperUpdater(ui_.ImproperTable, species_->impropers(), this,
                                                                        &SpeciesTab::updateImproperTableRow);
    }
    ui_.BondTable->resizeColumnsToContents();
    ui_.AngleTable->resizeColumnsToContents();
    ui_.TorsionTable->resizeColumnsToContents();
    ui_.ImproperTable->resizeColumnsToContents();
}
