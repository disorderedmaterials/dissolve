// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QStackedWidget>

// Super Tab Bar
class SuperStackedWidget : public QStackedWidget
{
    Q_OBJECT

    public:
    SuperStackedWidget(QWidget *parent);

    protected:
    void paintEvent(QPaintEvent *event);
};
