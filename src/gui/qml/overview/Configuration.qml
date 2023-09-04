import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0

Node {
    property variant model

    ColumnLayout {
        spacing: 0

        RowLayout {
            Text {
                text: `${model.data(model.index(index, 0), Qt.DisplayRole)}`
            }
            Image {
                Layout.preferredHeight: 15
                Layout.preferredWidth: 15
                source: "qrc:/general/icons/configuration.svg"
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
