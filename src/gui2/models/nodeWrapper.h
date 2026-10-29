// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "gui2/models/parameterModel.h"
#include "gui2/models/plot/plot1DModel.h"
#include "gui2/models/plot/renderableModel.h"
#include "math/histogram1D.h"
#include "nodes/node.h"
#include "templates/doubleKeyedMap.h"
#include <QAbstractListModel>
#include <QPointF>
#include <QVariant>
#include <map>

// A wrapper with supplemental information for a node
class NodeWrapper
{
    inline static std::unique_ptr<RenderableModel> &&addRenderables(const Node::NodeParameterMap &outputs)
    {
        auto model = std::make_unique<RenderableModel>();
        for (const auto &[name, parameter] : outputs)
        {
            // Handle Data1D source
            if (parameter->storedDataType() == typeid(Data1D))
                model->setData(model->index(model->rowCount(), 0),
                               QVariant::fromValue<RenderableModel::RenderableData>(
                                   {name, [parameter]() -> const Data1D * { return parameter->getPtr<Data1D>(); }}));

            // Handle Histogram1D source
            if (parameter->storedDataType() == typeid(Histogram1D))
                model->setData(model->index(model->rowCount(), 0),
                               QVariant::fromValue<RenderableModel::RenderableData>(
                                   {name, [parameter]() -> const Data1D *
                                    { return &parameter->getPtr<Histogram1D>()->accumulatedData(); }}));

            // Handle PartialSet source
            if (parameter->storedDataType() == typeid(PartialSet))
            {
                model->setData(
                    model->index(model->rowCount(), 0),
                    QVariant::fromValue<RenderableModel::RenderableData>(
                        {"Total", [parameter]() -> const Data1D * { return &parameter->getPtr<PartialSet>()->total(); }}));

                // Do the same for bound and unbound totals and partials...
            }
        }

        return std::move(model);
    }

    public:
    NodeWrapper(Node *node)
        : node_(node), inputs(std::make_unique<ParameterModel>(node->inputs())),
          outputs(std::make_unique<ParameterModel>(node->outputs())),
          options(std::make_unique<ParameterModel>(node->options())), renderables(NodeWrapper::addRenderables(node->outputs()))
    {
    }

    // Parameter models for parameters of the node
    std::unique_ptr<ParameterModel> inputs, outputs, options;
    // Model for the node's renderable data
    std::unique_ptr<RenderableModel> renderables;
    // Relative positions of parameters with respect to the node
    std::map<std::string, QPointF> inputsPos, outputPos;

    public:
    // Return the wrapped node
    Node *node() { return node_; }
    const Node *node() const { return node_; }
    // Does this node contain other nodes?
    bool hasInner() { return dynamic_cast<Graph *>(node_) != nullptr; }
    //
    const RenderableModel *renderableModel() { return renderables.get(); }

    private:
    // Pointer to the node we're wrapping
    Node *node_;
};
