// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui/models/atomTypeModel.h"
#include "gui/models/commonTermTreeModel.h"
#include "gui/models/configurationModel.h"
#include "main/dissolve.h"
#include "nodes/graph.h"
#include "templates/optionalRef.h"
#include <QUrl>
#include <memory>

class DissolveModel : public QObject
{
    Q_OBJECT
    // The Master Bond Model
    Q_PROPERTY(const CommonBondModel *commonBondsModel READ commonBondsModel NOTIFY commonsChanged)
    // The Master Angle Model
    Q_PROPERTY(const CommonAngleModel *commonAnglesModel READ commonAnglesModel NOTIFY commonsChanged)
    // The Master Torsion Model
    Q_PROPERTY(const CommonTorsionModel *commonTorsionsModel READ commonTorsionsModel NOTIFY commonsChanged)
    // The Master Improper Model
    Q_PROPERTY(const CommonImproperModel *commonImpropersModel READ commonImpropersModel NOTIFY commonsChanged)
    // The Configuration Model
    Q_PROPERTY(const ConfigurationModel *configurationsModel READ configurationsModel NOTIFY configurationsChanged)
    // File to load
    Q_PROPERTY(QUrl file READ fileName WRITE loadInput NOTIFY modelsUpdated)
    // The main graph
    Q_PROPERTY(Graph *graph READ graph NOTIFY modelsUpdated)

    private:
    // Master terms model
    std::unique_ptr<MasterTermTreeModel> commons_ = nullptr;

    public:
    // Getter for filename
    QUrl fileName();
    // Setter for filename
    void loadInput(QUrl filename);
    // Getter for graph
    Graph *graph();

    Q_SIGNALS:
    // The models might've been updated
    void modelsUpdated();
    // The Master terms models have been replaced
    void commonsChanged();

    // Species model has been replaced
    void speciesChanged();
    // Configurations model has been replaced
    void configurationsChanged();
    // ModuleLayers model has been replaced
    void moduleLayersChanged();

    public Q_SLOTS:
    // Export to a file
    bool saveAs(QUrl filename);

    public:
    DissolveModel() = default;
    ~DissolveModel() = default;

    // The Master Bond Model
    const CommonBondModel *commonBondsModel() const;
    // The number of common bonds
    int nCommonBonds();
    // The Master Angle Model
    const CommonAngleModel *commonAnglesModel() const;
    // The number of common angles
    int nCommonAngles();
    // The Master Torsion Model
    const CommonTorsionModel *commonTorsionsModel() const;
    // The number of common torsions
    int nCommonTorsions();
    // The Master Improper Model
    const CommonImproperModel *commonImpropersModel() const;
    // The number of common impropers
    int nCommonImpropers();

    /*
     * Data
     */
    private:
    // The main Dissolve instance
    Dissolve *dissolve_ = nullptr;

    public:
    // Access dissolve reference
    Dissolve &dissolve();
    // Set reference to Dissolve
    void setDissolve(Dissolve &dissolve);
    // Update models
    void update();
};
