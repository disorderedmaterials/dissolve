// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/coreData.h"
#include "classes/configuration.h"
#include "classes/box.h"
#include <QMessageBox>

class PairPotentialCheck {

    public: 
    
        static double checkPairPotential(std::vector<std::unique_ptr<Configuration>>& currentConfigs, double pairPotentialRange);
        static bool displayPairPotentialWarning(QWidget *parent = nullptr);
    
        static double radius;

    private:
        
        static QMessageBox::StandardButton reply;


};