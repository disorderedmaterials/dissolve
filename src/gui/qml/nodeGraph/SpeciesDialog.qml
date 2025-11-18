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
                    implicitHeight: 400
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
                            onClicked: sp.addBond(bondI.currentText, bondJ.currentText);
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
                        implicitHeight: 400
                        model: sp.bonds
                        delegate: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 50
                            Text {
                                text: display
                                visible: currentColumn != 2
                            }
                            TextField {
                                text: display
                                visible: currentColumn == 2
                            }
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
