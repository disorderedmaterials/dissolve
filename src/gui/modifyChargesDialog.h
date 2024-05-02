// spdx-license-identifier: gpl-3.0-or-later
// copyright (c) 2024 team dissolve and contributors

#pragma once

#include "gui/models/modifyChargesModel.h"
#include <QCloseEvent>
#include <QDialog>
#include <QObject>
#include <QQuickWidget>

class ModifyChargesModel;

class ModifyChargesDialog : public QDialog
{
    Q_OBJECT

    public:
    ModifyChargesDialog(QWidget *parent, ModifyChargesModel *dialogModel, ModifyChargesModel::ModifyBy modify);
    ~ModifyChargesDialog() = default;

    private:
    QQuickWidget *view_{nullptr};
};
