// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "graphModel.h"
#include "nodeWrapper.h"

/**
   This model is for an example graph model that mostly exists as a
   proof of concept of the node graph framework.  It's used in the
   testing, but should not be used at any place in Dissolve proper.
   Once one of the production node graphs is completed, this example
   model can likely be deleted.

 **/

class CoreData;

// The contents of the example node
class nodeValue
{
    public:
    nodeValue(QVariant var = {});

    // The name of the node
    std::string name;
    // The actual data within the node
    std::variant<double, nodeValue *> value;
};

// The graph model for the example
typedef GraphModel<nodeValue> ExampleGraphModel;
