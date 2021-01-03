// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/dropwidget.hui"
#include <stdio.h>

DropWidget::DropWidget(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint | Qt::Popup) {}

/*
 * Reimplementations
 */

// Hide event
void DropWidget::hideEvent(QHideEvent *event) { emit(hidden()); }
