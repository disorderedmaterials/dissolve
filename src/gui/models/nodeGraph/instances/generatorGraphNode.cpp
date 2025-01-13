#include "gui/models/nodeGraph/instances/generatorGraphNode.h"
#include "generator/node.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"

// The value of the node
template <> QVariant nodeGetValue<GeneratorGraphNode>(const GeneratorGraphNode value)
{
    return std::visit(
        [](auto *arg) -> QVariant
        {
            if (arg)
                return QVariant::fromValue(arg);
            return {};
        },
        value.value);
}

// The name of the type (for delegate dispatch)
template <> std::string nodeTypeName<GeneratorGraphNode>(const GeneratorGraphNode &value)
{
    return std::visit(
        [](auto *arg) -> std::string
        {
            if (arg)
                return nodeTypeName(arg);
            return "Null Pointer!";
        },
        value.value);
}

// Link an indexed position on the source to an indexed position on the destination
template <>
bool nodeConnect<GeneratorGraphNode>(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destination,
                                     int destinationIndex)
{
    return false;
}

// Confirm that a connection is possible (e.g. types match and index isn't already connected)
template <>
bool nodeConnectable<GeneratorGraphNode>(const GeneratorGraphNode &source, int sourceIndex,
                                         const GeneratorGraphNode &destination, int destinationIndex)
{
    return false;
}

// Unlink an indexed position on the source to an indexed position on the destination
template <>
bool nodeDisconnect<GeneratorGraphNode>(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destination,
                                        int destinationIndex)
{
    return true;
}

// The path to the icon for the node
template <> std::string nodeTypeIcon<GeneratorGraphNode>(const GeneratorGraphNode &value)
{
    return std::visit([](auto *arg) { return nodeTypeIcon(arg); }, value.value);
}

// The title of the node
std::string nodeName(GeneratorGraphNode &value)
{
    return std::visit([](auto *arg) { return nodeName(arg); }, value.value);
}

// Change the title of the node
template <> void setNodeName<GeneratorGraphNode>(GeneratorGraphNode &value, const std::string name) {}

// Set a specific piece of information from a node by index
template <> bool nodeSetData<GeneratorGraphNode>(GeneratorGraphNode &item, const QVariant &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    return std::visit([&value, role](auto *arg) { return nodeSetData(arg, value, role); }, item.value);
}

// Get a specific piece of information from a node by index
template <> QVariant nodeData(const GeneratorGraphNode &item, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    return std::visit([role](auto *arg) -> QVariant { return nodeData(arg, role); }, item.value);
}

// Append the roles for the type onto the QHash
template <> QHash<int, QByteArray> &nodeRoleNames<GeneratorGraphNode>(QHash<int, QByteArray> &roles)
{
    auto base = Qt::UserRole + GraphNodeModelBase::ownedRoles;
    using names = GeneratorGraphModel::PropertyIndex;
    roles[base + names::Value] = "value";
    roles[base + names::Size] = "size";
    roles[base + names::Temperature] = "temperature";
    roles[base + names::AtomicDensity] = "atomicDensity";
    return roles;
}

// Delete the node
template <>
bool nodeDelete<GeneratorGraphNode>(GeneratorGraphNode &item, typename GraphNodeContext<GeneratorGraphNode>::type &coreData)
{
    return std::visit([&](auto *arg) -> bool { return nodeDelete(arg, coreData); }, item.value);
}
