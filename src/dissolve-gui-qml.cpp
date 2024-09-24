// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/processPool.h"
#include "gui/models/dissolveModel.h"
#include "gui/models/types.h"
#include "gui/models/nodeGraph/graphModel.h"
#include "main/cli.h"
#include "main/dissolve.h"
#include "main/version.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>
#include <clocale>

int main(int args, char **argv)
{
    // Instantiate main classes
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Parse CLI options
    CLIOptions options;
    if (options.parse(args, argv, true) != CLIOptions::Success)
        return 1;

    // Initialise random seed
    srand(options.randomSeed().value_or((unsigned)time(nullptr)));

    QGuiApplication app(args, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/Dissolve/qml/DissolveMain.qml"_qs);
    Types::registerDissolveQmlTypes();

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    QCoreApplication::setOrganizationName("Team Dissolve");
    QCoreApplication::setOrganizationDomain("www.projectdissolve.com");
    QCoreApplication::setApplicationName("Dissolve-GUI-QML");

    // Set native siblings attribute to prevent odd rendering artefacts on some systems
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    // Ensure that the C locale is set, otherwise printf() and friends may not use dot for the radix point
    setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale::C);

    // Print GPL license information
    Messenger::print("Dissolve-GUI-QML {} version {}, Copyright (C) 2024 Team Dissolve and contributors.\n", Version::appType(),
                     Version::info());
    Messenger::print("Source repository: {}.\n", Version::repoUrl());
    Messenger::print("Dissolve comes with ABSOLUTELY NO WARRANTY.\n");
    Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

    // Flag to track our success in loading the input file and restart file
    auto loadSuccessful = true;

    // Set restart file frequency
    dissolve.setRestartFileFrequency(options.noRestartFile() ? 0 : options.restartFileFrequency());

    // If an input file was specified, load it here
    // if (options.inputFile())
    // loadSuccessful = dissolveWindow.loadInputFile(options.inputFile().value());

    // Load restart file if input file load was specified and loaded successfully
    if (options.inputFile() && loadSuccessful)
    {
        if (options.ignoreRestartFile())
            Messenger::print("Restart file (if it exists) will be ignored.\n");
        else
        {
            auto actualRestartFile{options.restartFilename().value_or(std::string(dissolve.restartFilename()))};
            // loadSuccessful = dissolveWindow.loadRestartFile(actualRestartFile);
        }

        // Iterate before launching the GUI?
        if (loadSuccessful && options.nIterations() > 0)
        {
            // Prepare for run
            if (!dissolve.prepare())
                return 1;

            // Run main simulation
            dissolve.iterate(options.nIterations());
        }
    }

    auto result = app.exec();

    // Done.
    return result;
}
