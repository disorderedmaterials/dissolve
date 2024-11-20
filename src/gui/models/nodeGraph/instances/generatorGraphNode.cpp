#include "gui/modes/nodeGraph/instances/generatorGraphNode.h"
#include "generator/node.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
// #include "gui/models/nodeGraph/nodeWrapper.h"

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

template <>
bool nodeConnect<GeneratorGraphNode>(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destination,
                                     int destinationIndex)
{
    return true;
}

template <>
bool nodeConnectable<GeneratorGraphNode>(const GeneratorGraphNode &source, int sourceIndex,
                                         const GeneratorGraphNode &destination, int destinationIndex)
{
    return false;
}

template <>
bool nodeDisconnect<GeneratorGraphNode>(GeneratorGraphNode &source, int sourceIndex, GeneratorGraphNode &destination,
                                        int destinationIndex)
{
    return true;
}

template <> std::string nodeTypeIcon<GeneratorGraphNode>(const GeneratorGraphNode &value)
{
    return std::visit([](auto *arg) { return nodeTypeIcon(arg); }, value.value);
}

template <> std::string nodeName<GeneratorGraphNode>(const GeneratorGraphNode &value)
{
    return std::visit([](auto *arg) { return nodeName(arg); }, value.value);
}

template <> void setNodeName<GeneratorGraphNode>(GeneratorGraphNode &value, const std::string name) {}

template <> bool nodeSetData<GeneratorGraphNode>(GeneratorGraphNode &item, const QVariant &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    return std::visit([&value, role](auto *arg) { return nodeSetData(arg, value, role); }, item.value);
}

template <> QVariant nodeData(const GeneratorGraphNode &item, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    return std::visit([role](auto *arg) -> QVariant { return nodeData(arg, role); }, item.value);
}

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

template <>
bool nodeDelete<GeneratorGraphNode>(GeneratorGraphNode &item, typename GraphNodeContext<GeneratorGraphNode>::type &coreData)
{
    return std::visit([&](auto *arg) -> bool { return nodeDelete(arg, coreData); }, item.value);
}
