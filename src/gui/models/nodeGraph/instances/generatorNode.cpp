#include "generator/node.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

template <> std::string nodeTypeName<GeneratorNode *>(GeneratorNode *const &value) { return "GeneratorNode"; }

template <> std::string nodeTypeIcon<GeneratorNode *>(GeneratorNode *const &value)
{
    auto name = GeneratorNode::nodeTypes().keyword(value->type());
    return "file:/home/adam/Code/dissolve/src/gui/icons/nodes/" + name + ".svg";
}

template <> std::string nodeName<GeneratorNode *>(GeneratorNode *const &value)
{
    std::string result = {value->name().begin(), value->name().end()};
    return result;
}

template <> QVariant nodeData(GeneratorNode *const &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        default:
            return {};
    }
}

template <> bool nodeSetData(GeneratorNode *&item, const QVariant &value, int role) { return false; }
