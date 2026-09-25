// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "utility.h"

Utility::Utility(QObject *parent) : QObject(parent) {}

QString Utility::urlToLocalFile(QUrl url) const
{
    return url.toLocalFile();
}