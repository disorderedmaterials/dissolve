// spdx-license-identifier: gpl-3.0-or-later
// copyright (c) 2024 team dissolve and contributors

#pragma once

#include "gui/models/modifyChargesModel.h"
#include <QObject>
#include <QDialog>
#include <QQuickWidget>
#include <QCloseEvent>

class ModifyChargesDialog : public QDialog
{
    Q_OBJECT

    public:
    ModifyChargesDialog(QWidget *parent, ModifyChargesModel *dialogModel, ModifyChargesModel::Modify modify);
    ~ModifyChargesDialog() = default;

    private:
    QQuickWidget *view_{nullptr};

    void initializeSpinBox(QObject*);
};
