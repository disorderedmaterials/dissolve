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
                    columns: 2
                    spacing: 2

                    Text {text: "Lengths"}

                    Text {text: "Angles"}

                    Text {
                        id: root
                        text: "Non-Periodic"
                    }
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
