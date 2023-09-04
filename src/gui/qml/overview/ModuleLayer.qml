import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0

Node {
    property variant model
    Text {
        text: `${model.data(model.index(index, 0), Qt.DisplayRole)}`
    }
}
