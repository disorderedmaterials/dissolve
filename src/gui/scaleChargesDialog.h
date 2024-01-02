// spdx-license-identifier: gpl-3.0-or-later
// copyright (c) 2023 team dissolve and contributors

#pragma once

#include "gui/models/scaleChargesDialogModel.h"
#include <QDialog>

class ScaleChargesDialog : public QDialog
{
    Q_OBJECT

    public:
    ScaleChargesDialog(QWidget *parent);
    ~ScaleChargesDialog() = default;

    ScaleChargesDialogModel *model;
};
