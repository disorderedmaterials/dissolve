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
            title: "Atoms (" + sp.atoms.rowCount() + ")"
            ColumnLayout {
                RowLayout {
                    Button {
                        text: "+"
                        onClicked: sp.atoms.addAtom(elementField.text, xField.text, yField.text, zField.text, chargeField.text);
                    }
                    Text { text: "Element" }
                    TextField { id: elementField }
                    Text { text: "X" }
                    TextField { id: xField }
                    Text { text: "Y" }
                    TextField { id: yField }
                    Text { text: "Z" }
                    TextField { id: zField }
                    Text { text: "Charge" }
                    TextField { id: chargeField }
                }
                TableView {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    implicitHeight: 400
                    clip: true
                    columnSpacing: 1
                    rowSpacing: 1

                    model: sp.atoms

                    delegate: Rectangle {
                        color: "white"
                        implicitWidth: 100
                        implicitHeight: 50
                        border.width: 1
                        Text {
                            text: display
                            anchors.centerIn: parent
                        }
                    }
                }
            }
        }
        GridLayout {
            columns: 2
            GroupBox {
                title: "Bonds (" + sp.bonds.rowCount() + ")"
                TableView {
                    model: sp.atoms
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
