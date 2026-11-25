// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/messenger.h"
#include "gui2/models/dissolveModel.h"
#include "gui2/models/nodeRegistryModel.h"
#include "gui2/models/types.h"
#include "gui2/singletons/types.h"
#include "main/cli.h"
#include "main/version.h"
#include "math/mathFunc.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSurfaceFormat>
#include <clocale>

int main(int args, char **argv)
{
    // Instantiate main classes
    auto dissolveGraph = std::make_unique<DissolveGraph>();

    DissolveModel dissolveModel;
    dissolveModel.setDissolveGraph(dissolveGraph.get());

    NodeRegistryModel nodeRegistryModel;

    // Parse GUI options from the commane line
    GUIOptions options;
    if (options.parse(args, argv) != CLIOptions::Success)
        return 1;

    QGuiApplication app(args, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("dissolve", &dissolveModel);
    engine.rootContext()->setContextProperty("nodeRegistry", &nodeRegistryModel);
    const QUrl url(u"qrc:/Dissolve/qml/DissolveMain.qml"_qs);
    Types::registerDissolveQmlTypes();
    Types::registerDissolveQmlSingletonTypes();

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    QCoreApplication::setOrganizationName("Team Dissolve");
    QCoreApplication::setOrganizationDomain("www.projectdissolve.com");
    QCoreApplication::setApplicationName("Dissolve2-GUI");

    // Set native siblings attribute to prevent odd rendering artefacts on some systems
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    // Ensure that the C locale is set, otherwise printf() and friends may not use dot for the radix point
    setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale::C);

    // Print GPL license information
    Messenger::print("Dissolve2-GUI {} version {}, Copyright (C) 2026 Team Dissolve and contributors.\n", Version::appType(),
                     Version::info());
    Messenger::print("Source repository: {}.\n", Version::repoUrl());
    Messenger::print("Dissolve comes with ABSOLUTELY NO WARRANTY.\n");
    Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

    // If an input file was specified, load it here
    // TODO DISSOLVE2
    if (options.inputFile())
        dissolveModel.loadInput(QUrl::fromLocalFile(QString(options.inputFile()->c_str())));
    // auto loadSuccessful = dissolveWindow.loadInputFile(options.inputFile().value());

    auto result = app.exec();

    // Done.
    return result;
}
