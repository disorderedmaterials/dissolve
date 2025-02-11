#include "gui/models/nodeGraph/instances/generatorNode.h"
#include "generator/node.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/keywordModel.h"
#include "gui/models/nodeGraph/keywords.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

// The name of the type (for delegate dispatch)
std::string nodeTypeName(GeneratorNode *const &value) { return value->nodeTypes().keyword(value->type()); }

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

void setNodeName(GeneratorNode *value, std::string name) { value->setName(name); }

// Get a specific piece of information from a node by index
QVariant nodeData(GeneratorNode *value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        case names::KeywordModel:
        {
            // auto model = std::make_shared<KeywordModel>(&value->keywords());
            auto model = new KeywordModel(&value->keywords());
            return QVariant::fromValue(model);
        }
        default:
            return {};
    }
}

// Set a specific piece of information from a node by index
bool nodeSetData(GeneratorNode *item, const QVariant &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!item)
        return {};
    switch (role)
    {
        case names::Value:
            return false;
        default:
            return {};
    }
}

// Delete the node
bool nodeDelete(GeneratorNode *item, CoreData *coreData)
{
    for (auto &conf : coreData->configurations())
    {
        conf->generator().removeNode(item->shared_from_this());
    }
    return true;
}

QVariant nodeGetValue(const GeneratorNode *value) { return QVariant::fromValue(value); }

bool nodeConnect(const GeneratorNode *source, int sourceIndex, const GeneratorNode *destionation, int destinationIndex)
{
    // Connections not yet implemented
    return false;
}

bool nodeConnectable(const GeneratorNode *source, int sourceIndex, const GeneratorNode *destination, int destinationIndex)
{
    // Connections not yet implemented
    return false;
}

bool nodeDisconnect(const GeneratorNode *source, int sourceIndex, const GeneratorNode *destination, int destinationIndex)
{
    // Connections not yet implemented
    return false;
}

QHash<int, QByteArray> &nodeRoleNames(Phantom<GeneratorNode *> proxy, QHash<int, QByteArray> &roles)
{

    using names = GeneratorGraphModel::PropertyIndex;
    const auto base = Qt::UserRole + GraphNodeModelBase::ownedRoles;
    roles[base + names::KeywordModel] = "keywordModel";
    return roles;
}
