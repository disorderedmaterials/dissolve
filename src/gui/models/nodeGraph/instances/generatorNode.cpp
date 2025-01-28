#include "gui/models/nodeGraph/instances/generatorNode.h"
#include "generator/node.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
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
QVariant nodeData(const GeneratorNode *value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        case names::Temperature:
            return unlockKeyword<names::Temperature>(value->keywords());
        case names::LengthsA:
            return unlockKeyword<names::LengthsA>(value->keywords());
        case names::LengthsB:
            return unlockKeyword<names::LengthsB>(value->keywords());
        case names::LengthsC:
            return unlockKeyword<names::LengthsC>(value->keywords());
        case names::AnglesA:
            return unlockKeyword<names::AnglesA>(value->keywords());
        case names::AnglesB:
            return unlockKeyword<names::AnglesB>(value->keywords());
        case names::AnglesC:
            return unlockKeyword<names::AnglesC>(value->keywords());
        case names::NonPeriodic:
            return unlockKeyword<names::NonPeriodic>(value->keywords());
        case names::Species:
            return unlockKeyword<names::Species>(value->keywords());
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
        case names::Temperature:
            return updateKeyword<names::Temperature>(item->keywords(), value);
        case names::LengthsA:
            return updateKeyword<names::LengthsA>(item->keywords(), value);
        case names::LengthsB:
            return updateKeyword<names::LengthsB>(item->keywords(), value);
        case names::LengthsC:
            return updateKeyword<names::LengthsC>(item->keywords(), value);
        case names::AnglesA:
            return updateKeyword<names::AnglesA>(item->keywords(), value);
        case names::AnglesB:
            return updateKeyword<names::AnglesB>(item->keywords(), value);
        case names::AnglesC:
            return updateKeyword<names::AnglesC>(item->keywords(), value);
        case names::NonPeriodic:
            return updateKeyword<names::NonPeriodic>(item->keywords(), value);
        case names::Species:
            return updateKeyword<names::Species>(item->keywords(), value);
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
    roles[base + names::Temperature] = "temperature";
    roles[base + names::LengthsA] = "lengthsA";
    roles[base + names::LengthsB] = "lengthsB";
    roles[base + names::LengthsC] = "lengthsC";
    roles[base + names::AnglesA] = "anglesA";
    roles[base + names::AnglesB] = "anglesB";
    roles[base + names::AnglesC] = "anglesC";
    roles[base + names::NonPeriodic] = "nonPeriodic";
    roles[base + names::Species] = "species";
    return roles;
}
