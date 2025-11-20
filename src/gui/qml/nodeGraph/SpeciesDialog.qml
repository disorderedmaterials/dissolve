import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Dissolve

Dialog {
    id: root

    required property variant graphModel

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
            Text {
                text: "Name"
            }
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
                    Text {
                        text: "Element"
                    }
                    ElementComboBox {
                        id: elementField

                    }
                    Text {
                        text: "X"
                    }
                    TextField {
                        id: xField

                        validator: DoubleValidator {
                        }
                    }
                    Text {
                        text: "Y"
                    }
                    TextField {
                        id: yField

                        validator: DoubleValidator {
                        }
                    }
                    Text {
                        text: "Z"
                    }
                    TextField {
                        id: zField

                        validator: DoubleValidator {
                        }
                    }
                    Text {
                        text: "Charge"
                    }
                    TextField {
                        id: chargeField

                        validator: DoubleValidator {
                        }
                    }
                    Button {
                        enabled: xField.text != "" && yField.text != "" && zField.text != "" && chargeField.text != ""
                        text: "+"

                        onClicked: sp.atoms.addAtom(elementField.currentText, xField.text, yField.text, zField.text, chargeField.text)
                    }
                }
                TableView {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    clip: true
                    columnSpacing: 1
                    implicitHeight: 200
                    implicitWidth: zField.width * sp.atoms.columnCount()
                    model: sp.atoms
                    rowSpacing: 1

                    delegate: TextField {
                        implicitHeight: 20
                        implicitWidth: parent.parent.width / 6 - 1
                        text: display

                        onTextChanged: edit = text
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
                        Text {
                            text: "I"
                        }
                        TextField {
                            id: bondI

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "J"
                        }
                        TextField {
                            id: bondJ

                            validator: IntValidator {
                            }
                        }
                        Button {
                            enabled: bondI.text != "" && bondJ.text != ""
                            text: "+"

                            onClicked: sp.addBond(bondI.text, bondJ.text)
                        }
                    }
                    TableView {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        implicitHeight: 200
                        implicitWidth: bondI.width * sp.bonds.columnCount()
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
                        Text {
                            text: "I"
                        }
                        TextField {
                            id: angleI

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "J"
                        }
                        TextField {
                            id: angleJ

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "K"
                        }
                        TextField {
                            id: angleK

                            validator: IntValidator {
                            }
                        }
                        Button {
                            enabled: angleI.text != "" && angleJ.text != "" && angleK.text != ""
                            text: "+"

                            onClicked: sp.addAngle(angleI.text, angleJ.text, angleK.text)
                        }
                    }
                    TableView {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        implicitHeight: 200
                        implicitWidth: angleI.width * sp.angles.columnCount()
                        model: sp.angles

                        delegate: TextField {
                            text: display
                            width: 10

                            onTextChanged: edit = text
                        }
                    }
                }
            }
            GroupBox {
                title: "Torsions"

                ColumnLayout {
                    RowLayout {
                        Text {
                            text: "I"
                        }
                        TextField {
                            id: torsionI

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "J"
                        }
                        TextField {
                            id: torsionJ

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "K"
                        }
                        TextField {
                            id: torsionK

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "L"
                        }
                        TextField {
                            id: torsionL

                            validator: IntValidator {
                            }
                        }
                        Button {
                            enabled: torsionI.text != "" && torsionJ.text != "" && torsionK.text != "" && torsionL.text != ""
                            text: "+"

                            onClicked: sp.addTorsion(torsionI.text, torsionJ.text, torsionK.text, torsionL.text)
                        }
                    }
                    TableView {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        implicitHeight: 200
                        implicitWidth: torsionI.width * sp.torsions.columnCount()
                        model: sp.torsions

                        delegate: TextField {
                            text: display
                            width: 10

                            onTextChanged: edit = text
                        }
                    }
                }
            }
            GroupBox {
                title: "Impropers"

                ColumnLayout {
                    RowLayout {
                        Text {
                            text: "I"
                        }
                        TextField {
                            id: improperI

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "J"
                        }
                        TextField {
                            id: improperJ

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "K"
                        }
                        TextField {
                            id: improperK

                            validator: IntValidator {
                            }
                        }
                        Text {
                            text: "L"
                        }
                        TextField {
                            id: improperL

                            validator: IntValidator {
                            }
                        }
                        Button {
                            enabled: improperI.text != "" && improperJ.text != "" && improperK.text != "" && improperL.text != ""
                            text: "+"

                            onClicked: sp.addImproper(improperI.text, improperJ.text, improperK.text, improperL.text)
                        }
                    }
                    TableView {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        implicitHeight: 200
                        implicitWidth: improperI.width * sp.impropers.columnCount()
                        model: sp.impropers

                        delegate: TextField {
                            text: display
                            width: 10

                            onTextChanged: edit = text
                        }
                    }
                }
            }
        }
    }
}
