// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"
#include <string>

// Forward Declarations
/* none */

// Reference Point
class ReferencePoint : public ListItem<ReferencePoint>
{
    public:
    ReferencePoint();
    ~ReferencePoint();

    /*
     * Data
     */
    private:
    // Suffix applied to data items from the referenced file
    std::string suffix_;
    // Restart file from which the reference point data was read
    std::string restartFile_;

    public:
    // Set suffix for data items
    void setSuffix(std::string_view suffix);
    // Return suffix for data items
    std::string_view suffix() const;
    // Set restart file from which the reference point data was read
    void setRestartFile(std::string_view restartFile);
    // Return restart file from which the reference point data was read
    std::string_view restartFile() const;
};
