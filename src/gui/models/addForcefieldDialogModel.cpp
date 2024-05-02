// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/addForcefieldDialogModel.h"
#include "data/ff/library.h"
#include <QMessageBox>
#include <vector>

// Instantiate the model
AddForcefieldDialogModel::AddForcefieldDialogModel() : ffSort_(this)
{
    ffModel_ = std::make_unique<ForcefieldModel>(ForcefieldLibrary::forcefields());

    ffSort_.setSourceModel(ffModel_.get());
}

AddForcefieldDialogModel::~AddForcefieldDialogModel() { temporaryDissolve_->clear(); }

// The current page in the wizard
AddForcefieldDialogModel::Page AddForcefieldDialogModel::index() { return index_; }

// Regress in the wizard
void AddForcefieldDialogModel::back()
{
    switch (index_)
    {
        case AddForcefieldDialogModel::Page::MasterTermsPage:
            index_ = AddForcefieldDialogModel::Page::IntramolecularPage;
            break;
        case AddForcefieldDialogModel::Page::IntramolecularPage:
            index_ = AddForcefieldDialogModel::Page::AtomTypesConflictsPage;
            break;
        case AddForcefieldDialogModel::Page::AtomTypesConflictsPage:
            index_ = AddForcefieldDialogModel::Page::AtomTypesPage;
            break;
        case AddForcefieldDialogModel::Page::AtomTypesPage:
            index_ = AddForcefieldDialogModel::Page::SelectForcefieldPage;
            break;
        default:
            index_ = AddForcefieldDialogModel::Page::SelectForcefieldPage;
    }
    indexChanged();
}

// Progress in the wizard
void AddForcefieldDialogModel::next()
{
    std::vector<int> assignErrs;
    switch (index_)
    {
        case AddForcefieldDialogModel::Page::SelectForcefieldPage:
            index_ = AddForcefieldDialogModel::Page::AtomTypesPage;
            Q_EMIT assignErrors({});
            break;
        case AddForcefieldDialogModel::Page::AtomTypesPage:
            if (!ff_) // No valud forcefield
                return;
            modifiedSpecies_ = temporaryDissolve_->coreData().addSpecies();
            modifiedSpecies_->copyBasic(species_);
            originalAtomTypeNames_.clear();

            // Set selection status
            for (auto &&[targetI, modifiedI] : zip(species_->atoms(), modifiedSpecies_->atoms()))
                modifiedI.setSelected(targetI.isSelected());

            Q_EMIT assignErrors({});
            // Determine atom types
            switch (atomTypeRadio_)
            {
                case Radio::All:
                    modifiedSpecies_->clearAtomTypes();
                    temporaryDissolve_->coreData().clearAtomTypes();

                    assignErrs = ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeAll,
                                                      !keepSpeciesAtomChargesCheck_);
                    break;
                case Radio::Selected:
                    assignErrs = ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeSelection,
                                                      !keepSpeciesAtomChargesCheck_);
                    break;
                case Radio::Empty:
                    assignErrs = ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeMissing,
                                                      !keepSpeciesAtomChargesCheck_);
                default:
                    break;
            }

            if (!assignErrs.empty())
            {
                Q_EMIT assignErrors({assignErrs.begin(), assignErrs.end()});
                return;
            }

            for (auto &at : temporaryDissolve_->coreData().atomTypes())
                originalAtomTypeNames_.emplace_back(std::string(at->name()));

            atomTypes_.setData(temporaryCoreData_.atomTypes());
            Q_EMIT atomTypesIndicatorChanged();

            // checkAtomTypeConflicts();
            index_ = AddForcefieldDialogModel::Page::AtomTypesConflictsPage;
            break;
        case AddForcefieldDialogModel::Page::AtomTypesConflictsPage:
            index_ = AddForcefieldDialogModel::Page::IntramolecularPage;
            break;
        case AddForcefieldDialogModel::Page::IntramolecularPage:
            Q_EMIT mastersChanged();
            index_ = AddForcefieldDialogModel::Page::MasterTermsPage;
            break;
        case AddForcefieldDialogModel::Page::MasterTermsPage:
            finalise();
            break;
        default:
            index_ = AddForcefieldDialogModel::Page::SelectForcefieldPage;
    }
    indexChanged();
}

// Supply the main Dissolve instance
void AddForcefieldDialogModel::setDissolve(Dissolve &dissolve)
{
    dissolve_ = &dissolve;

    temporaryDissolve_ = std::make_unique<Dissolve>(temporaryCoreData_);
    auto node = dissolve_->coreData().serialiseMaster();
    temporaryCoreData_.deserialiseMaster(node);
    masters_ = std::make_unique<MasterTermTreeModel>();
    masters_->setData(temporaryCoreData_.masterBonds(), temporaryCoreData_.masterAngles(), temporaryCoreData_.masterTorsions(),
                      temporaryCoreData_.masterImpropers());
    // Set model and signals for the master terms tree
    atomTypes_.setIconFunction([this](const auto type) { return dissolve_->coreData().findAtomType(type->name()) != nullptr; });
    masters_->setBondIconFunction([this](std::string_view name)
                                  { return dissolve_->coreData().getMasterBond(name).has_value(); });
    masters_->setAngleIconFunction([this](std::string_view name)
                                   { return dissolve_->coreData().getMasterAngle(name).has_value(); });
    masters_->setTorsionIconFunction([this](std::string_view name)
                                     { return dissolve_->coreData().getMasterTorsion(name).has_value(); });
    masters_->setImproperIconFunction([this](std::string_view name)
                                      { return dissolve_->coreData().getMasterImproper(name).has_value(); });
}

// Supply the species to operate on
void AddForcefieldDialogModel::setSpecies(Species *sp)
{
    species_ = sp;
    ffSort_.setSpecies(species_);
}

// Does the species have selected atoms
bool AddForcefieldDialogModel::speciesHasSelection() const
{
    if (!species_)
        return false;
    return !species_->selectedAtoms().empty();
}

// The forcefield model
const QAbstractItemModel *AddForcefieldDialogModel::forcefields() const { return &ffSort_; }

// The Atom Type Model
AtomTypeModel *AddForcefieldDialogModel::atomTypes() { return &atomTypes_; }

// Can the user safely pass to the next stage?
bool AddForcefieldDialogModel::progressionAllowed() const
{
    if (index_ == Page::SelectForcefieldPage)
        return ff_ != nullptr;
    return true;
}

// Determine whether we have any naming conflicts
int AddForcefieldDialogModel::atomTypesIndicator() const
{
    return std::count_if(temporaryCoreData_.atomTypes().begin(), temporaryCoreData_.atomTypes().end(),
                         [&](const auto &atomType) { return dissolve_->coreData().findAtomType(atomType->name()); });
}

// Whether we are at the final page of the wizard
bool AddForcefieldDialogModel::atEnd() const { return index_ == Page::MasterTermsPage; }

// The Master Bond Model
const MasterBondModel *AddForcefieldDialogModel::bonds() const
{
    if (!masters_)
        return nullptr;
    return &masters_->bondModel_;
}

// The Master Angle Model
const MasterAngleModel *AddForcefieldDialogModel::angles() const
{
    if (!masters_)
        return nullptr;
    return &masters_->angleModel_;
}

// The Master Torsion Model
const MasterTorsionModel *AddForcefieldDialogModel::torsions() const
{
    if (!masters_)
        return nullptr;
    return &masters_->torsionModel_;
}

// The Master Improper Model
const MasterImproperModel *AddForcefieldDialogModel::impropers() const
{
    if (!masters_)
        return nullptr;
    return &masters_->improperModel_;
}

// Apply the forcefield
void AddForcefieldDialogModel::finalise()
{
    /*
     * We have the original Species (which should exist in the provided Dissolve object) in currentSpecies()
     * and the temporary species modifiedSpecies_ which contains the new Forcefield terms.
     *
     * So, we will assume that the ordering of all data (atoms, bonds etc.) in the two are the same, and then copy and reassign
     * AtomTypes and intramolecular terms.
     */

    auto typesSelectionOnly = atomTypeRadio_ == Radio::Selected;
    auto intraSelectionOnly = intramolecularRadio_ == Radio::Selected;

    // 1) Set AtomTypes
    for (const auto &&[original, modified] : zip(species_->atoms(), modifiedSpecies_->atoms()))
    {

        // Selection only?
        if (typesSelectionOnly && (!original.isSelected()))
            continue;

        // Copy AtomType
        dissolve_->coreData().copyAtomType(modified, original);

        // Overwrite existing parameters?
        if (overwriteParametersCheck_)
        {
            original.atomType()->interactionPotential() = modified.atomType()->interactionPotential();
            original.atomType()->setCharge(modified.atomType()->charge());
            dissolve_->coreData().bumpAtomTypesVersion();
        }

        // Copy charge on species atom
        original.setCharge(modified.charge());
    }

    // Assign intramolecular terms if we need to
    assignIntramolecularTerms(ff_);

    // Copy intramolecular terms
    if (intramolecularRadio_ != Radio::None)
    {
        auto modifiedBond = modifiedSpecies_->bonds().cbegin();
        for (auto &originalBond : species_->bonds())
        {
            // Selection only?
            if (intraSelectionOnly && (!originalBond.isSelected()))
                continue;

            dissolve_->coreData().copySpeciesBond(*modifiedBond, originalBond);

            ++modifiedBond;
        }

        auto modifiedAngle = modifiedSpecies_->angles().cbegin();
        for (auto &originalAngle : species_->angles())
        {
            // Selection only?
            if (intraSelectionOnly && (!originalAngle.isSelected()))
                continue;

            dissolve_->coreData().copySpeciesAngle(*modifiedAngle, originalAngle);

            ++modifiedAngle;
        }

        auto modifiedTorsion = modifiedSpecies_->torsions().cbegin();
        for (auto &originalTorsion : species_->torsions())
        {
            // Selection only?
            if (intraSelectionOnly && (!originalTorsion.isSelected()))
                continue;

            dissolve_->coreData().copySpeciesTorsion(*modifiedTorsion, originalTorsion);

            ++modifiedTorsion;
        }

        for (auto &modifiedImproper : modifiedSpecies_->impropers())
        {
            // Selection only?
            if (intraSelectionOnly && (!modifiedImproper.isSelected()))
                continue;

            // Find / create the improper in the target species
            auto optImproper = species_->getImproper(modifiedImproper.indexI(), modifiedImproper.indexJ(),
                                                     modifiedImproper.indexK(), modifiedImproper.indexL());
            if (optImproper)
                dissolve_->coreData().copySpeciesImproper(modifiedImproper, *optImproper);
            else
            {
                auto &improper = species_->addImproper(modifiedImproper.indexI(), modifiedImproper.indexJ(),
                                                       modifiedImproper.indexK(), modifiedImproper.indexL());
                dissolve_->coreData().copySpeciesImproper(modifiedImproper, improper);
            }
        }
    }
    accept();
}

void AddForcefieldDialogModel::addMasterSuffix(int type, int index, QString suffix)
{
    QVariant termData;
    switch (type)
    {
        case 0:
            termData = masters_->bondModel_.getTermData(index, MasterTermModelData::DataType::Name);
            break;
        case 1:
            termData = masters_->angleModel_.getTermData(index, MasterTermModelData::DataType::Name);
            break;
        case 2:
            termData = masters_->torsionModel_.getTermData(index, MasterTermModelData::DataType::Name);
            break;
        case 3:
            termData = masters_->improperModel_.getTermData(index, MasterTermModelData::DataType::Name);
            break;
    };
    auto newName = termData.toString() + suffix;
    switch (type)
    {
        case 0:
            termData = masters_->bondModel_.setTermData(index, MasterTermModelData::DataType::Name, newName);
            break;
        case 1:
            termData = masters_->angleModel_.setTermData(index, MasterTermModelData::DataType::Name, newName);
            break;
        case 2:
            termData = masters_->torsionModel_.setTermData(index, MasterTermModelData::DataType::Name, newName);
            break;
        case 3:
            termData = masters_->improperModel_.setTermData(index, MasterTermModelData::DataType::Name, newName);
            break;
    };
}

void AddForcefieldDialogModel::addMasterPrefix(int type, int index, QString prefix)
{
    QVariant termData;
    switch (type)
    {
        case 0:
            termData = masters_->bondModel_.getTermData(index, MasterTermModelData::DataType::Name);
            break;
        case 1:
            termData = masters_->angleModel_.getTermData(index, MasterTermModelData::DataType::Name);
            break;
        case 2:
            termData = masters_->torsionModel_.getTermData(index, MasterTermModelData::DataType::Name);
            break;
        case 3:
            termData = masters_->improperModel_.getTermData(index, MasterTermModelData::DataType::Name);
            break;
    };
    auto newName = prefix + termData.toString();
    switch (type)
    {
        case 0:
            termData = masters_->bondModel_.setTermData(index, MasterTermModelData::DataType::Name, newName);
            break;
        case 1:
            termData = masters_->angleModel_.setTermData(index, MasterTermModelData::DataType::Name, newName);
            break;
        case 2:
            termData = masters_->torsionModel_.setTermData(index, MasterTermModelData::DataType::Name, newName);
            break;
        case 3:
            termData = masters_->improperModel_.setTermData(index, MasterTermModelData::DataType::Name, newName);
            break;
    };
}

// The chosen forcefield
Forcefield *AddForcefieldDialogModel::ff() const { return ff_; }

// Update the chosen forcefield
void AddForcefieldDialogModel::setFf(Forcefield *f)
{
    ff_ = f;
    Q_EMIT progressionAllowedChanged();
}

// Assign intramolecular terms to species
void AddForcefieldDialogModel::assignIntramolecularTerms(const Forcefield *ff)
{
    // Detach any MasterTerm references, and delete the MasterTerms
    modifiedSpecies_->detachFromMasterTerms();
    temporaryCoreData_.clearMasterTerms();

    // Assign intramolecular terms
    if (intramolecularRadio_ != Radio::None)
    {
        auto flags = 0;
        if (ignoreCurrentTypes_)
            flags += Forcefield::DetermineTypesFlag;
        if (!noImproperTerms_)
            flags += Forcefield::GenerateImpropersFlag;
        if (intramolecularRadio_ == Radio::Selected)
            flags += Forcefield::SelectionOnlyFlag;

        if (!ff->assignIntramolecular(modifiedSpecies_, flags))
            // Assigning failed
            return;

        // Reduce to master terms?
        if (!noMasterTerms_)
            modifiedSpecies_->reduceToMasterTerms(temporaryCoreData_, intramolecularRadio_ == Radio::Selected);
    }
    return;
}
