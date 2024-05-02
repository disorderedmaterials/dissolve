#include "gui/models/globalPotentialFilterProxy.h"

bool GlobalPotentialFilterProxy::filterAcceptsColumn(int column, const QModelIndex &parent) const { return column != 3; }