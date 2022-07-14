// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/datamanagerdialog.h"
#include "gui/gui.h"
#include "main/dissolve.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

void DissolveWindow::on_SimulationCheckAction_triggered(bool checked)
{
    clearMessages();

    dissolve_.prepare();

    ui_.MainTabs->forcefieldTab()->resetPairPotentialModel();

    fullUpdate();
}

void DissolveWindow::on_SimulationRunAction_triggered(bool checked) { setupIteration(-1); }

void DissolveWindow::on_SimulationRunForAction_triggered(bool checked)
{
    // Get the number of iterations to run
    static auto nIterations = 10;
    bool ok;
    auto newNIterations =
        QInputDialog::getInt(this, "Iterate Simulation...", "Enter the number of iterations to run", 10, 1, 1000000, 10, &ok);
    if (!ok)
        return;
    nIterations = newNIterations;
    setupIteration(nIterations);
}

void DissolveWindow::on_SimulationStepAction_triggered(bool checked) { setupIteration(1); }

void DissolveWindow::on_SimulationStepFiveAction_triggered(bool checked) { setupIteration(5); }

void DissolveWindow::on_SimulationStopAction_triggered(bool checked) { emit(stopIterating()); }

void DissolveWindow::on_SimulationSetRestartFileFrequencyAction_triggered(bool checked)
{
    auto ok{false};
    auto newFrequency =
        QInputDialog::getInt(this, "Set Restart File Frequency", "Enter the new frequency at which to write the restart file",
                             dissolve_.restartFileFrequency(), 0, 10000000, 5, &ok);
    if (!ok)
        return;
    dissolve_.setRestartFileFrequency(newFrequency);
}

void DissolveWindow::on_SimulationDataManagerAction_triggered(bool checked)
{
    DataManagerDialog dataManagerDialog(this, dissolve_, dissolve_.processingModuleData());
    dataManagerDialog.exec();
}

void DissolveWindow::on_SimulationClearAdditionalPotentialsAction_triggered(bool checked)
{
    if (QMessageBox::warning(this, "Clear Additional Potentials",
                             "This will reset any generate additional (empirical) potentials, and reset the pair potentials to "
                             "the reference parameters.\n\n"
                             "This cannot be undone! Proceed?",
                             QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                             QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
    {
        dissolve_.revertPairPotentials();

        fullUpdate();
    }
}

void DissolveWindow::on_SimulationClearModuleDataAction_triggered(bool checked) { clearModuleData(); }
