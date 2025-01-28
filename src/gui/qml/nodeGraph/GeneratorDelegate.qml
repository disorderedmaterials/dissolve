import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

Item {
    property variant delegate: DelegateChooser {
        role: "type"

        DelegateChoice {
            roleValue: "Temperature"

            delegate: NodeBox {
                property double endX: x
                property double midY: y + basey + root.y + root.height / 2 + 10
                property double startX: x + width

                image: icon
                nodeType: name
                px: posX
                py: posY
                x: posX
                y: posY

                onDeleted: rootModel.deleteNode(index)

                Grid {
                    columns: 2
                    spacing: 2

                    Text {text: "Temperature"}

                    Text {
                        id: root
                        text: temperature
                    }
                }
            }
        }
        DelegateChoice {
            roleValue: "Box"

            delegate: NodeBox {
                property double endX: x
                property double midY: y + basey + root.y + root.height / 2 + 10
                property double startX: x + width

                image: icon
                nodeType: name
                px: posX
                py: posY
                x: posX
                y: posY

                onDeleted: rootModel.deleteNode(index)

                Grid {
                    columns: 4
                    spacing: 2

                    Text {text: "Lengths"}

                    Text {text: lengthsA}
                    Text {text: lengthsB}
                    Text {text: lengthsC}

                    Text {text: "Angles"}

                    Text {text: anglesA}
                    Text {text: anglesB}
                    Text {text: anglesC}

                    Text {
                        id: root
                        text: "Non-Periodic"
                    }

                    CheckBox {checked: nonPeriodic}
                }
            }
        }
        DelegateChoice {
            roleValue: "Add"

            delegate: NodeBox {
                property double endX: x
                property double midY: y + basey + root.y + root.height / 2 + 10
                property double startX: x + width

                image: icon
                nodeType: name
                px: posX
                py: posY
                x: posX
                y: posY

                onDeleted: rootModel.deleteNode(index)

                Grid {
                    columns: 2
                    spacing: 2

                    Text {
                        id: root
                        text: "Species"
                    }

                    Text {
                        text: species
                    }

                    Text {
                        text: "Population"
                    }
                }
            }
        }
        DelegateChoice {
            roleValue: "Parameters"

            delegate: NodeBox {
                property double endX: x
                property double midY: y + basey + root.y + root.height / 2 + 10
                property double startX: x + width

                image: icon
                nodeType: name
                px: posX
                py: posY
                x: posX
                y: posY

                onDeleted: rootModel.deleteNode(index)

                Grid {
                    columns: 2
                    spacing: 2

                    Text {text: "Name"}
                    Text {
                        id: root
                        text: "Value"
                    }
                }
            }
        }
    }
    property variant rootModel
}
