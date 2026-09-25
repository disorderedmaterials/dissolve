// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/singletons/types.h"
#include "gui2/singletons/utility.h"
#include <QQmlEngine>

void Types::registerDissolveQmlSingletonTypes()
{
    qmlRegisterSingletonType<Utility>(PROJECT, 1, 0, "Utility", [](QQmlEngine *, QJSEngine*) -> QObject * { return new Utility(); });
}