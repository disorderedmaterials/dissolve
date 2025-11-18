import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Dissolve


Dialog {
    id: root

    required property variant graphModel;

    standardButtons: Dialog.Ok | Dialog.Cancel
    title: "Define Species: " + sp.name

    onAccepted: {
        sp.create(graphModel);
    }

    SpeciesModel {
        id: sp
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
            ColumnLayout {
                RowLayout {
                    Button {
                        text: "+"
                        enabled: xField.text != "" && yField.text!= "" && zField.text!= "" && chargeField.text != "";
                        onClicked: sp.atoms.addAtom(elementField.currentText, xField.text, yField.text, zField.text, chargeField.text);
                    }
                    Text { text: "Element" }
                    ElementComboBox { id:elementField }
                    Text { text: "X" }
                    TextField {
                        id: xField
                        validator: DoubleValidator {}
                    }
                    Text { text: "Y" }
                    TextField {
                        id: yField
                        validator: DoubleValidator {}
                    }
                    Text { text: "Z" }
                    TextField {
                        id: zField
                        validator: DoubleValidator {}
                    }
                    Text { text: "Charge" }
                    TextField {
                        id: chargeField
                        validator: DoubleValidator {}
                    }
                }
                TableView {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    implicitHeight: 200
                    clip: true
                    columnSpacing: 1
                    rowSpacing: 1

                    model: sp.atoms

                    delegate: Rectangle {
                        color: "white"
                        implicitWidth: parent.parent.width / 6 - 1
                        implicitHeight: 20
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
                title: "Bonds"
                ColumnLayout {
                    RowLayout {
                        Button {
                            text: "+"
                            enabled: bondI.text != "" && bondJ.text!= "";
                            onClicked: sp.addBond(bondI.text, bondJ.text);
                        }
                        Text { text: "I" }
                        TextField {
                            id: bondI
                            validator: IntValidator {}
                        }
                        Text { text: "J" }
                        TextField {
                            id: bondJ
                            validator: IntValidator {}
                        }
                    }
                    TableView {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        implicitHeight: 200
                        implicitWidth: bondI.width*4
                        model: sp.bonds
                        delegate: TextField {
                            text: display
                            width: 10
                            onTextChanged: edit = text
                        }
                    }
                }
            }
            GroupBox {
                title: "Angles"
                ColumnLayout {
                    RowLayout {
                        Button {
                            text: "+"
                            enabled: angleI.text != "" && angleJ.text!= "";
                            onClicked: sp.addAngle(angleI.text, angleJ.text, angleK);
                        }
                        Text { text: "I" }
                        TextField {
                            id: angleI
                            validator: IntValidator {}
                        }
                        Text { text: "J" }
                        TextField {
                            id: angleJ
                            validator: IntValidator {}
                        }
                        Text { text: "K" }
                        TextField {
                            id: angleK
                            validator: IntValidator {}
                        }
                    }
                    TableView {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        implicitHeight: 200
                        implicitWidth: parent.parent.width/2
                        model: sp.angles
                        delegate: TextField {
                            text: display
                            color: "#00FFFF"
                            width: 20
                            onTextChanged: edit = text
                        }
                    }
                }
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
