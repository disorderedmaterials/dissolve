#include <functional>
#include <map>
#include <memory>
#include <string>
#include "atomShake.h"
#include "node.h"

using NodeProducer = std::function<std::unique_ptr<Node> (void)>;

const std::map<std::string, NodeProducer> registry{
	{
        "AtomShake", []() -> std::unique_ptr<Node> { return std::make_unique<AtomShakeNode>(); }
    },
    // etc...
}