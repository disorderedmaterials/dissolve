import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0

Node {
    property variant model
    property variant modelData

    ColumnLayout {
        spacing: 0
        RowLayout {
            Text {
                text: `${model.data(model.index(index, 0), Qt.DisplayRole)}`
            }
            Image {
                source: "qrc:/general/icons/configuration.svg"
                Layout.preferredWidth: 15
                Layout.preferredHeight: 15
            }
        }
        Text {
            text: `Molecules: ${model.data(model.index(index, 0), ConfigurationModel.ConfigurationUserRole.MoleculesCount)}`
        }
        Text {
            text: `Atoms: ${model.data(model.index(index, 0), ConfigurationModel.ConfigurationUserRole.AtomsCount)}`
        }
    }
}
