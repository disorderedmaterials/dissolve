// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/dropWidget.hui"
#include <stdio.h>

DropWidget::DropWidget(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint | Qt::Popup) {}

/*
 * Reimplementations
 */

// Hide event
void DropWidget::hideEvent(QHideEvent *event) { emit(hidden()); }
