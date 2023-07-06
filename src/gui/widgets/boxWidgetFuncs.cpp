// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/units.h"
#include "gui/widgets/boxWidget.h"
#include "classes/box.h"
#include "gui/helpers/combopopulator.h"

BoxWidget::BoxWidget(QWidget *parent, Configuration* cfg) : QWidget(parent), configuration_(cfg)
{
    ui_.setupUi(this);

    // Populate density units combo
    ComboEnumOptionsPopulator(ui_.DensityUnitsCombo, Units::densityUnits());

    installEventFilter(this);



}

BoxWidget::~BoxWidget() {}

void BoxWidget::update()
{

    // Populations
    ui_.AtomPopulation->setText(QString::number(configuration_->nAtoms()));
    ui_.MolPopulation->setText(QString::number(configuration_->nMolecules()));

    // Current Box
    const auto *box = configuration_->box();
    ui_.CurrentBoxTypeLabel->setText(QString::fromStdString(std::string(Box::boxTypes().keyword(box->type()))));
    ui_.CurrentBoxALabel->setText(QString::number(box->axisLengths().x));
    ui_.CurrentBoxBLabel->setText(QString::number(box->axisLengths().y));
    ui_.CurrentBoxCLabel->setText(QString::number(box->axisLengths().z));
    ui_.CurrentBoxAlphaLabel->setText(QString::number(box->axisAngles().x));
    ui_.CurrentBoxBetaLabel->setText(QString::number(box->axisAngles().y));
    ui_.CurrentBoxGammaLabel->setText(QString::number(box->axisAngles().z));
    updateDensityLabel();
}

/*
 * Event filer
 */
void BoxWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    QPoint globalMousePos = QCursor::pos();
    QPoint widgetMousePos = mapFromGlobal(globalMousePos);

    if (!rect().contains(widgetMousePos))
    {
        setHidden(true);
    }
}

bool BoxWidget::eventFilter(QObject *obj, QEvent *event)
{
    /*if (event->type() == QEvent::Leave)
    {
        bool isUnderMouse = std::any_of(children().begin(), children().end(), [](QObject* child) {
            if (child->isWidgetType()) {
                QWidget* childWidget = static_cast<QWidget*>(child);
                return childWidget->underMouse();
            }
            return false;
        });

        if (!isUnderMouse && !underMouse())
        {
            setHidden(true);
        }
    }
    */
    return QObject::eventFilter(obj, event);
}

// Update density label
void BoxWidget::updateDensityLabel()
{
    if (!configuration_)
        ui_.DensityUnitsLabel->setText("N/A");
    else
    {
        auto rho =
            ui_.DensityUnitsCombo->currentIndex() == 0 ? configuration_->atomicDensity() : configuration_->chemicalDensity();
        ui_.DensityUnitsLabel->setText(rho ? QString::number(*rho) : "--");
    }
}

void BoxWidget::on_DensityUnitsCombo_currentIndexChanged(int index) { updateDensityLabel(); }
