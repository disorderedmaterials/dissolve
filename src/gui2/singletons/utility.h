// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <QObject>
#include <QUrl>
#include <QString>

class Utility : public QObject
{
    Q_OBJECT;

    public:
    explicit Utility(QObject *parent = nullptr);
    ~Utility() = default;

    Q_INVOKABLE QString urlToLocalFile(QUrl url) const;
};