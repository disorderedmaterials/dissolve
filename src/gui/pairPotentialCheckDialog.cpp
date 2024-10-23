// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "pairPotentialCheckDialog.h"

double PairPotentialCheck::radius = 0.0;
QMessageBox::StandardButton PairPotentialCheck::reply;

// Retrieve the inscribed sphere radius for all configuration boxes find the smallest
// Compare smallest radius to pair potential range, return radius if smaller

double PairPotentialCheck::checkPairPotential(std::vector<std::unique_ptr<Configuration>>& currentConfigs, double pairPotentialRange) {

    radius = pairPotentialRange;

    for (const auto& config : currentConfigs) {
        
        if (config->box()->inscribedSphereRadius() < radius) {

            radius = config->box()->inscribedSphereRadius();
        }
    }

    if (radius == pairPotentialRange)
        return 0;
    else 
        return radius;

}

// Prompt the user for automatic adjustment, return the choice as true/false

bool PairPotentialCheck::displayPairPotentialWarning(QWidget *parent) {

    reply = QMessageBox::question(parent, "Warning!", QString("Maximum pair potential range exceeds smallest box inradius! Adjust pair potential range to %1? and run?").arg(radius),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        
    if (reply == QMessageBox::Yes)
        return true;
    else 
        return false;
    
}