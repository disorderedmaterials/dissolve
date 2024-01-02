import Dissolve
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root

    property double value

    font.pointSize: 10
    visible: true
    title: "Scale Charges"
    width: 655
    height: 116

    ScaleChargesDialogModel {
        id: dialogModel

        objectName: "dialogModel"
    }

    contentItem: Rectangle {
        width: root.width
        height: root.height

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: "Enter the scaling factor to apply to all atoms / the target sum to determine scaling factor from"
                font.pixelSize: 10
            }

            SpinBox {
                id: scaleSpinBox

                editable: true
                from: -100
                to: 100
                value: dialogModel.value
                stepSize: 1
                onValueChanged: {
                    dialogModel.updateValue(scaleSpinBox.value);
                }
            }

            Row {
                spacing: 10

                Rectangle {
                    width: 40
                    height: 20
                }

                Button {
                    id: cancelButton

                    text: "Cancel"
                    onClicked: {
                        dialogModel.cancelSelection();
                    }
                }

                Rectangle {
                    width: 40
                    height: 20
                }

                Button {
                    id: scaleButton

                    text: "Scale"
                    onClicked: {
                        dialogModel.setOption(1);
                        dialogModel.acceptSelection();
                    }
                }

                Rectangle {
                    width: 40
                    height: 20
                }

                Button {
                    id: scaleToButton

                    text: "Scale To"
                    onClicked: {
                        dialogModel.setOption(2);
                        dialogModel.acceptSelection();
                    }
                }

            }

        }

    }

}
