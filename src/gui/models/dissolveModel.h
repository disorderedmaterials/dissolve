// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "gui/models/atomTypeModel.h"
#include "gui/models/configurationModel.h"
#include "gui/models/masterTermTreeModel.h"
#include "gui/models/speciesModel.h"
#include "templates/optionalRef.h"
#include <memory>

class DissolveModel : public QObject
{
    Q_OBJECT
    // The Atom Type Model
    Q_PROPERTY(AtomTypeModel *atomTypes READ atomTypes NOTIFY atomTypesChanged)
    // The Master Bond Model
    Q_PROPERTY(const MasterBondModel *bonds READ bonds NOTIFY mastersChanged)
    // The Master Angle Model
    Q_PROPERTY(const MasterAngleModel *angles READ angles NOTIFY mastersChanged)
    // The Master Torsion Model
    Q_PROPERTY(const MasterTorsionModel *torsions READ torsions NOTIFY mastersChanged)
    // The Master Improper Model
    Q_PROPERTY(const MasterImproperModel *impropers READ impropers NOTIFY mastersChanged)
    // The Species Model
    Q_PROPERTY(const SpeciesModel *species READ species NOTIFY speciesChanged)
    // The Configuration Model
    Q_PROPERTY(const ConfigurationModel* configurations READ configurations NOTIFY configurationsChanged)

    private:
    // The Atom Type Model
    AtomTypeModel atomTypes_;
    // Master terms model
    std::unique_ptr<MasterTermTreeModel> masters_ = nullptr;
    SpeciesModel speciesModel_;
    ConfigurationModel configurationModel_;

    signals:
    // The Atom Types model has been replaced
    void atomTypesChanged();
    // The Master terms models have been replaced
    void mastersChanged();
    // Species model has been replaced
    void speciesChanged();
    // Configurations model has been replaced
    void configurationsChanged();

    public:
    DissolveModel() = default;
    ~DissolveModel() = default;

    // The Atom Type Model
    AtomTypeModel *atomTypes();
    // The Master Bond Model
    const MasterBondModel *bonds() const;
    // The Master Angle Model
    const MasterAngleModel *angles() const;
    // The Master Torsion Model
    const MasterTorsionModel *torsions() const;
    // The Master Improper Model
    const MasterImproperModel *impropers() const;
    // The Species Model
    SpeciesModel *species();
    // The Configurations Model
    ConfigurationModel *configurations();

    /*
     * Data
     */
    private:
    // The main Dissolve instance
    Dissolve *dissolve_ = nullptr;

    public:
    // Set reference to Dissolve
    void setDissolve(Dissolve &dissolve);
    void reset();
};
