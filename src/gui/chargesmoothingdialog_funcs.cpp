#include "base/sysfunc.h"
#include "gui/chargesmoothingdialog.h"

ChargeSmoothingDialog::ChargeSmoothingDialog(QWidget *parent) { ui_.setupUi(this); }

bool ChargeSmoothingDialog::normalise() const { return ui_.NormaliseCheckBox->checkState() == Qt::CheckState::Checked; }

int ChargeSmoothingDialog::normalisationTarget() const {return ui_.NormalisationTargetSpin->value(); }

bool ChargeSmoothingDialog::roundOff() const { return ui_.RoundOffCheckBox->checkState() == Qt::CheckState::Checked; }

int ChargeSmoothingDialog::significantFigures() const { return ui_.SignificantFiguresSpin->value(); }

void ChargeSmoothingDialog::on_CancelButton_clicked(bool checked) { reject(); }

void ChargeSmoothingDialog::on_OKButton_clicked(bool checked) { accept(); }

