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
                source: "qrc:/general/icons/species.svg"
                Layout.preferredWidth: 15
                Layout.preferredHeight: 15
            }
        }
        Text {
            text: `Bonds: ${model.data(model.index(index, 0), SpeciesModel.SpeciesUserRole.BondsCount)}`
        }
        Text {
            text: `Angles: ${model.data(model.index(index, 0), SpeciesModel.SpeciesUserRole.AnglesCount)}`
        }
        Text {
            text: `Torsions: ${model.data(model.index(index, 0), SpeciesModel.SpeciesUserRole.TorsionsCount)}`
        }
        Text {
            text: `Impropers: ${model.data(model.index(index, 0), SpeciesModel.SpeciesUserRole.ImpropersCount)}`
        }

    }
}