#include "gui/models/modelUpdater.h"
#include <QAbstractItemModel>

void ModelUpdater::connectModelSignals(QAbstractItemModel *model)
{
    connect(model, &columnsAboutToBeInserted, this, &update);
    connect(model, &columnsAboutToBeMoved, this, &update);
    connect(model, &columnsAboutToBeRemoved, this, &update);
    connect(model, &columnsInserted, this, &update);
    connect(model, &columnsMoved, this, &update);
    connect(model, &columnsRemoved, this, &update);
    connect(model, &dataChanged, this, &update);
    connect(model, &headerDataChanged, this, &update);
    connect(model, &layoutAboutToBeChanged, this, &update);
    connect(model, &layoutChanged, this, &update);
    connect(model, &modelAboutToBeReset, this, &update);
    connect(model, &modelReset, this, &update);
    connect(model, &rowsABoutToBeInserted, this, &update);
    connect(model, &rowsABoutToBeMoved, this, &update);
    connect(model, &rowsABoutToBeRemoved, this, &update);
    connect(model, &rowsInserted, this, &update);
    connect(model, &rowsMoved, this, &update);
    connect(model, &rowsRemoved, this, &update);
}

void ModelUpdater::update() { emit modelsUpdated(); }