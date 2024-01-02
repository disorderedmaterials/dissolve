// spdx-license-identifier: gpl-3.0-or-later
// copyright (c) 2023 team dissolve and contributors

#pragma once

#include "gui/ui_scaleChargesDialog.h"
#include <QDialog>

class ScaleChargesDialog : public QDialog
{
    Q_OBJECT

    Q_PROPERTY(double value READ value NOTIFY valueSet)

    public:
    ScaleChargesDialog(QWidget *parent);
    ~ScaleChargesDialog();
    bool scale_, scaleTo_;

    Q_INVOKABLE void setOption(int);
    Q_INVOKABLE void updateValue(double);

    double value() const;

    private:
    double currentValue_=1.0;

    signals:
    void valueSet();
    void cancelSelection();
    void acceptSelection();
};
