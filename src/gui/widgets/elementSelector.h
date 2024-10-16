// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include <QTimer>
#include <QWidget>

// Forward Declarations
class QToolButton;

// Element Selector Widget
class ElementSelector : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ElementSelector(QWidget *parent);
    ~ElementSelector();

    /*
     * Widget Functions
     */
    private:
    // Timer to enable detection of double-click events on buttons
    QTimer doubleClickTimer_;

    private Q_SLOTS:
    void elementButtonClicked(bool checked);

    Q_SIGNALS:
    void elementSelectionChanged();
    void elementSelected(bool);
    void elementDoubleClicked();

    /*
     * Data
     */
    private:
    // Current element
    Elements::Element currentElement_;
    // Vector of toole buttons corresponding to elements
    std::vector<QToolButton *> buttons_;

    public:
    // Set current element
    void setCurrentElement(Elements::Element Z);
    // Return current element
    Elements::Element currentElement() const;

    /*
     * Static Functions
     */
    public:
    // Get Element from user via input dialog
    static Elements::Element getElement(QWidget *parent, QString title, QString labelText, Elements::Element Z,
                                        bool *ok = nullptr, Qt::WindowFlags flags = Qt::Dialog);
};
