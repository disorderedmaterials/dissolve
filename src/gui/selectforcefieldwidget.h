// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/library.h"
#include "gui/ui_selectforcefieldwidget.h"
#include "templates/list.h"
#include <QWidget>

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr)
Q_DECLARE_METATYPE(std::shared_ptr<Forcefield>)

// Forward Declarations
class Dissolve;
class Forcefield;

// Select Forcefield Widget
class SelectForcefieldWidget : public QWidget
{
    Q_OBJECT

    public:
    SelectForcefieldWidget(QWidget *parent,
                           const std::vector<std::shared_ptr<Forcefield>> &forcefields = ForcefieldLibrary::forcefields());
    ~SelectForcefieldWidget();

    private:
    // Main form declaration
    Ui::SelectForcefieldWidget ui_;
    // Available forcefields
    const std::vector<std::shared_ptr<Forcefield>> &forcefields_;
    // Whether the widget is refreshing
    bool refreshing_;

    private:
    // Update the list of Forcefields, optionally filtering them by name and description
    void updateForcefieldsList(std::shared_ptr<Forcefield> current = nullptr, QString filter = QString());

    private slots:
    void on_FilterEdit_textChanged(const QString &text);
    void on_ForcefieldsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_ForcefieldsList_itemDoubleClicked(QListWidgetItem *item);

    signals:
    void forcefieldSelectionChanged(bool isValid);
    void forcefieldDoubleClicked();

    public:
    // Set the current forcefield
    void setCurrentForcefield(std::shared_ptr<Forcefield> currentFF);
    // Return the currently-selected Forcefield
    std::shared_ptr<Forcefield> currentForcefield() const;
};
