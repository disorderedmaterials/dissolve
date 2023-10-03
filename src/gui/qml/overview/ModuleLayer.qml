import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0
import "../widgets" as D

Node {
    id: moduleLayer
    property variant model

    ColumnLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 0

        RowLayout {
            id: banner
            Text {
                text: `${model.data(model.index(index, 0), Qt.DisplayRole)}`
            }
            Image {
                Layout.preferredHeight: 15
                Layout.preferredWidth: 15
                source: "qrc:/general/icons/layer.svg"
            }
        }
        ScrollView {
            Layout.preferredHeight: moduleLayer.height - banner.height
            Layout.preferredWidth: moduleLayer.width
            clip: true

            D.PrettyListView {
                iconPrefix: "image://dissolveIcons/moduleLayer/" + index + "/"
                model: moduleLayer.model.data(moduleLayer.model.index(index, 0), ModuleLayersModel.ModuleLayersUserRole.ModuleLayerModel)
            }
        }
    }
}
