import Dissolve
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

Dialog {
    id: root

    anchors.fill: parent
    font.pointSize: 10
    visible: true
    title: "Scale Charges"
    width: 655
    height: 116

    contentItem: Rectangle {
        width: root.width
        height: root.height

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                text: "Enter the scaling factor to apply to all atoms / the target sum to determine scaling factor from"
                font.pixelSize: 16
            }

            SpinBox {
                id: scaleSpinBox

                editable: true
                from: -100
                to: 100
                value: root.value
                stepSize: 1
                onValueChanged: {
                    root.updateValue(scaleSpinBox.value);
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
                        root.cancel();
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
                        root.scale();
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
                        root.scaleTo();
                    }
                }

            }

        }

    }

}
