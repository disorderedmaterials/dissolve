#include "procedure/nodes/node.h"

#pragma once

NodeRef nodeGenerator(const SerialisedValue &node, const CoreData &data, const std::shared_ptr<ProcedureNode> parent = nullptr);
