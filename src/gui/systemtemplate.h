// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"
#include <QDir>
#include <QString>

// Forward Declarations
class QToolButton;

// System Template
class SystemTemplate : public ListItem<SystemTemplate>
{
    public:
    SystemTemplate();
    ~SystemTemplate();
    SystemTemplate(const SystemTemplate &source);
    void operator=(const SystemTemplate &source);

    /*
     * Data
     */
    private:
    // Name of the template
    QString name_;
    // Group in which the template exists
    QString group_;
    // Short description of the template
    QString description_;
    // Resource path to the template's icon
    QString iconResource_;
    // Template's input data
    QString inputFileData_;
    // QToolButton created for the template
    QToolButton *toolButton_;

    public:
    // Read data contained in specified dir
    bool read(const QDir rootResourceDir);
    // Return name of the template
    QString name() const;
    // Return group in which the template exists
    QString group() const;
    // Return short description of the template
    QString description() const;
    // Return resource path to the template's icon
    QString iconResource() const;
    // Return the template's input data
    QString inputFileData() const;
    // Return QToolButton created for the template
    QToolButton *toolButton() const;
    // Create and return a button for this template
    QToolButton *createButton();
};
