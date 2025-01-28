#include "gui/models/nodeGraph/instances/generatorNode.h"
#include "generator/node.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/nodeWrapper.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/nodeValue.h"
#include "keywords/vec3NodeValue.h"

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

template <GeneratorGraphModel::PropertyIndex T> QVariant unlockKeyword(const KeywordStore &keywords);

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::Temperature>(const KeywordStore &keywords)
{
    auto temp = keywords.find("Temperature");
    // Check that we found a keyword
    if (!temp)
        return {};
    auto as_double = dynamic_cast<NodeValueKeyword *>(temp.value().first);
    // Check that the value is a double
    if (!as_double)
        return {};
    return QVariant::fromValue(as_double->data().asDouble());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::LengthsA>(const KeywordStore &keywords)
{
    auto temp = keywords.find("Lengths");
    // Check that we found a keyword
    if (!temp)
        return {};
    auto as_double = dynamic_cast<Vec3NodeValueKeyword *>(temp.value().first);
    // Check that the value is a double
    if (!as_double)
        return {};
    auto data = as_double->data();
    return QVariant::fromValue(data.get(0).asDouble());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::LengthsB>(const KeywordStore &keywords)
{
    auto temp = keywords.find("Lengths");
    // Check that we found a keyword
    if (!temp)
        return {};
    auto as_double = dynamic_cast<Vec3NodeValueKeyword *>(temp.value().first);
    // Check that the value is a double
    if (!as_double)
        return {};
    auto data = as_double->data();
    return QVariant::fromValue(data.get(1).asDouble());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::LengthsC>(const KeywordStore &keywords)
{
    auto temp = keywords.find("Lengths");
    // Check that we found a keyword
    if (!temp)
        return {};
    auto as_double = dynamic_cast<Vec3NodeValueKeyword *>(temp.value().first);
    // Check that the value is a double
    if (!as_double)
        return {};
    auto data = as_double->data();
    return QVariant::fromValue(data.get(2).asDouble());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::AnglesA>(const KeywordStore &keywords)
{
    auto temp = keywords.find("Angles");
    // Check that we found a keyword
    if (!temp)
        return {};
    auto as_double = dynamic_cast<Vec3NodeValueKeyword *>(temp.value().first);
    // Check that the value is a double
    if (!as_double)
        return {};
    auto data = as_double->data();
    return QVariant::fromValue(data.get(0).asDouble());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::AnglesB>(const KeywordStore &keywords)
{
    auto temp = keywords.find("Angles");
    // Check that we found a keyword
    if (!temp)
        return {};
    auto as_double = dynamic_cast<Vec3NodeValueKeyword *>(temp.value().first);
    // Check that the value is a double
    if (!as_double)
        return {};
    auto data = as_double->data();
    return QVariant::fromValue(data.get(1).asDouble());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::AnglesC>(const KeywordStore &keywords)
{
    auto temp = keywords.find("Angles");
    // Check that we found a keyword
    if (!temp)
        return {};
    auto as_double = dynamic_cast<Vec3NodeValueKeyword *>(temp.value().first);
    // Check that the value is a double
    if (!as_double)
        return {};
    auto data = as_double->data();
    return QVariant::fromValue(data.get(2).asDouble());
}

template <> QVariant unlockKeyword<GeneratorGraphModel::PropertyIndex::NonPeriodic>(const KeywordStore &keywords)
{
    auto temp = keywords.find("NonPeriodic");
    // Check that we found a keyword
    if (!temp)
        return {};
    auto as_double = dynamic_cast<BoolKeyword *>(temp.value().first);
    // Check that the value is a double
    if (!as_double)
        return {};
    return QVariant::fromValue(as_double->data());
}

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
        default:
            return {};
    }
}

// Set a specific piece of information from a node by index
bool nodeSetData(GeneratorNode *item, const QVariant &value, int role) { return false; }

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
    return roles;
}
