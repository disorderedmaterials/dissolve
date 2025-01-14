#include "gui/models/nodeGraph/instances/generatorNode.h"
#include "generator/node.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/nodeWrapper.h"
#include "keywords/double.h"
#include "keywords/nodeValue.h"

// The name of the type (for delegate dispatch)
std::string nodeTypeName(GeneratorNode *const &value) { return "GeneratorNode"; }

// The path to the icon for the node
std::string nodeTypeIcon(GeneratorNode *const &value)
{
    auto name = GeneratorNode::nodeTypes().keyword(value->type());
    return "qrc:/Dissolve/icons/nodes/" + name + ".svg";
}

// The title of the node
std::string nodeName(GeneratorNode *const &value)
{
    std::string result = {value->name().begin(), value->name().end()};
    return result;
}

// Get a specific piece of information from a node by index
template <> QVariant nodeData(GeneratorNode *const &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        case names::Temperature:
        {
            auto temp = value->keywords().find("Temperature");
            // Check that we found a keyword
            if (!temp)
                return {};
            auto as_double = dynamic_cast<NodeValueKeyword *>(temp.value().first);
            // Check that the value is a double
            if (!as_double)
                return {};
            return QVariant::fromValue(as_double->data().asDouble());
        }
        default:
            return {};
    }
}

// Set a specific piece of information from a node by index
template <> bool nodeSetData(GeneratorNode *&item, const QVariant &value, int role) { return false; }

// Delete the node
template <> bool nodeDelete(GeneratorNode *&item, typename GraphNodeContext<GeneratorNode *>::type &coreData)
{
    for (auto &conf : coreData->configurations())
    {
        conf->generator().removeNode(item->shared_from_this());
    }
    return true;
}
