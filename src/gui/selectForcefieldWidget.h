// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/library.h"
#include "gui/ui_selectForcefieldWidget.h"
#include <QWidget>

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

    private Q_SLOTS:
    void on_FilterEdit_textChanged(const QString &text);
    void on_ForcefieldsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_ForcefieldsList_itemDoubleClicked(QListWidgetItem *item);

    Q_SIGNALS:
    void forcefieldSelectionChanged(bool isValid);
    void forcefieldDoubleClicked();

    public:
    // Set the current forcefield
    void setCurrentForcefield(std::shared_ptr<Forcefield> currentFF);
    // Return the currently-selected Forcefield
    std::shared_ptr<Forcefield> currentForcefield() const;
};
