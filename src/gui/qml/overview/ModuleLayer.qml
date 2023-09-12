import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0
import "../widgets" as D

Node {
    id: moduleLayer
    property variant model

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Text {
                text: `${model.data(model.index(index, 0), Qt.DisplayRole)}`
            }
            Image {
                Layout.preferredHeight: 15
                Layout.preferredWidth: 15
                source: "qrc:/general/icons/layer.svg"
            }
        }
        D.PrettyListView {
            height: contentHeight
            iconPrefix: "image://dissolveIcons/moduleLayer/" + index + "/"
            model: moduleLayer.model.data(moduleLayer.model.index(index, 0), ModuleLayersModel.ModuleLayersUserRole.ModuleLayerModel)
            width: contentWidth
        }
    }
}
