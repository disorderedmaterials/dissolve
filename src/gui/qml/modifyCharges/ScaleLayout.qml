import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../widgets" as D


ColumnLayout {
    anchors.fill: parent
    spacing: 10

    property ModifyChargesModel dialogModel
    property int modify
    property double scaleValue

    D.Text {
        Layout.fillHeight: true
        Layout.fillWidth: true
        font.pixelSize: 11
        text: "Enter the scaling factor to apply to all atoms / the target sum to determine scaling factor from"
        width: parent.width - 2 * parent.spacing
        wrapMode: Text.WordWrap
    }
    SpinBox {
        id: spinBox
        objectName: "spinBox"
        Layout.alignment: Qt.AlignRight
        Layout.fillWidth: true
        editable: true
        from: -100
        stepSize: 1
        to: 100
        value: dialogModel.scaleValue
    }
    RowLayout {
        Layout.alignment: Qt.AlignRight
        spacing: 10

        D.Button {
            id: cancelButton
            icon.source: "qrc:/general/icons/false.svg"
            text: "Cancel"

            onClicked: {
                dialogModel.cancelSelection();
                spinBox.value = dialogModel.scaleValue;
            }
        }
        D.Button {
            id: scaleButton
            icon.source: "qrc:/general/icons/true.svg"
            text: "Scale"

            onClicked: {
                dialogModel.setScaleType(dialogModel.Scale);
                dialogModel.updateScaleValue(spinBox.value);
                dialogModel.acceptSelection();
            }
        }
        D.Button {
            id: scaleToButton
            icon.source: "qrc:/general/icons/true.svg"
            text: "Scale To"

            onClicked: {
                dialogModel.setScaleType(dialogModel.ScaleTo);
                dialogModel.updateScaleValue(spinBox.value);
                dialogModel.acceptSelection();
            }
        }
    }
}
