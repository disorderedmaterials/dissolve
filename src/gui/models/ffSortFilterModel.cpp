// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/ffSortFilterModel.h"
#include "gui/models/forcefieldModel.h"
#include <memory>

ForcefieldSortFilterModel::ForcefieldSortFilterModel(QObject *parent, CoreData &coreData)
    : QSortFilterProxyModel(parent), temporaryCoreData_(coreData)
{
}

bool ForcefieldSortFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &source) const
{
    if (!species_)
	return false;

    auto index = sourceModel()->index(sourceRow, 0, source);
    QString name = sourceModel()->data(index).toString();
    auto regex = filterRegularExpression();
    regex.setPatternOptions(QRegularExpression::PatternOption::CaseInsensitiveOption);
    if (!regex.match(name).hasMatch())
      return false;
    Forcefield *ff = sourceModel()->data(index, ForcefieldModel::ffRoles::RawRole).value<Forcefield *>();
    if (!ff)
	return false;

    Dissolve temporaryDissolve(temporaryCoreData_);

    Species *modifiedSpecies_ = temporaryDissolve.addSpecies();
    modifiedSpecies_->copyBasic(species_);
    // // originalAtomTypeNames_.clear();

    modifiedSpecies_->clearAtomTypes();
    temporaryDissolve.clearAtomTypes();

    ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeAll, true);
    return temporaryDissolve.atomTypes().size() > 0;
}

void ForcefieldSortFilterModel::setSpecies(const Species *species)
{
    species_ = species;
    invalidateFilter();
}
