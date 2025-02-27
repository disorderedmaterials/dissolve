#include "atomShake.h"
#include "node.h"
#include <functional>
#include <map>
#include <memory>
#include <string>

using NodeProducer = std::function<std::unique_ptr<Node>(void)>;

// Makes unique pointer to derived node instance
template <typename T> NodeProducer makeDerivedNode()
{
    auto nodeLambda = []() -> std::unique_ptr<Node> { return std::make_unique<T>(); };
    return nodeLambda;
}

// Node registry
const std::map<std::string, NodeProducer> registry
{
    {"AtomShake", makeDerivedNode<AtomShakeNode>()},
    // etc...
}
