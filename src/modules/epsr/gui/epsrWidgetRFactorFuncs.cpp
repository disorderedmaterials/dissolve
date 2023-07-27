#include "gui/rFactorAddRangeDialog.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/epsrWidget.h"

void EPSRModuleWidget::setUpRFactorMenu()
{
    auto *rFactorAllAction = new QAction("All", this);
    auto *rFactorAddRangeAction = new QAction("Add Range", this);

    rFactorAddRangeAction->setIcon(QIcon(":/general/icons/general_add.svg"));

    QObject::connect(rFactorAllAction, SIGNAL(triggered(bool)), this, SLOT(rFactorAll()));
    QObject::connect(rFactorAddRangeAction, SIGNAL(triggered(bool)), this, SLOT(rFactorAddRange()));

    auto *rFactorMenu = new QMenu;
    rFactorMenu->addAction(rFactorAllAction);
    rFactorMenu->addAction(rFactorAddRangeAction);

    ui_.RFactorButton->setMenu(rFactorMenu);
}

void EPSRModuleWidget::rFactorAll() {}

void EPSRModuleWidget::rFactorAddRange()
{

    RFactorAddRangeDialog rFactorAddRangeDialog(this);

    if (rFactorAddRangeDialog.exec() == QDialog::Accepted)
    {
    }
}