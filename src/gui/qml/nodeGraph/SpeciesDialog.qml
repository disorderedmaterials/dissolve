import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Dissolve


Dialog {
    id: root
    required property variant species;

    standardButtons: Dialog.Ok
    title: "Define Species: " + sp.name

    SpeciesModel {
        id: sp
        species: root.species
    }
    ColumnLayout {
        RowLayout {
            Text {text: "Name"}
            TextField {
                placeholderText: sp.name
                onTextChanged: sp.name = text
            }
        }
        GroupBox {
            id: atomBox
            title: "Atoms"
        }
        GridLayout {
            columns: 2
            GroupBox {
                title: "Bonds"
                TableView {
                    model: sp.bonds
                    delegate: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 50
                        Text {
                            text: display
                        }
                    }
                }
            }
            GroupBox {
                title: "Angles"
            }
            GroupBox {
                title: "Torsions"
            }
            GroupBox {
                title: "Impropers"
            }
        }
    }
}
