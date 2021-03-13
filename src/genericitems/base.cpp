// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "genericitems/base.h"

/*
 * I/O
 */

// Write data through specified parser
bool GenericItemBase::write(LineParser &parser)
{
    return Messenger::error("Tried to write() a class that doesn't support it.\n");
}

// Read data through specified parser
bool GenericItemBase::read(LineParser &parser, const CoreData &coreData)
{
    return Messenger::error("Tried to read() a class that doesn't support it.\n");
}

/*
 * Parallel Comms
 */

// Broadcast item contents
bool GenericItemBase::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
    return Messenger::error("Tried to broadcast() a class that doesn't support it.\n");
}

// Return equality between items
bool GenericItemBase::equality(ProcessPool &procPool)
{
    return Messenger::error("Tried to equality() a class that doesn't support it.\n");
}
