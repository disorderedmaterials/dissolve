import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../widgets" as D

ColumnLayout {
    property ModifyChargesModel dialogModel
    property int modify
    property double scaleValue

    anchors.fill: parent
    spacing: 10

    D.Text {
        Layout.fillHeight: true
        Layout.fillWidth: true
        text: "Enter the scaling factor to apply to all atoms / the target sum to determine scaling factor from"
        width: parent.width - 2 * parent.spacing
        wrapMode: Text.WordWrap
    }
    SpinBox {
        id: spinBox
        readonly property int decimalFactor: Math.pow(10, decimals)
        property int decimals: 2
        property real realValue: value / 100

        Layout.alignment: Qt.AlignRight
        Layout.fillWidth: true
        editable: true
        from: decimalToInt(-100)
        objectName: "scaleSpinBox"
        stepSize: 1
        textFromValue: function (value) {
            return String((parseFloat(value) / decimalFactor).toFixed(decimals));
        }
        to: decimalToInt(100)
        value: decimalToInt(dialogModel.scaleValue)
        valueFromText: function (text) {
            return Math.round(parseFloat(text) * decimalFactor);
        }

        function decimalToInt(decimal) {
            return decimal * decimalFactor;
        }

        validator: DoubleValidator {
            bottom: Math.min(spinBox.from, spinBox.to)
            decimals: spinBox.decimals
            notation: DoubleValidator.StandardNotation
            top: Math.max(spinBox.from, spinBox.to)
        }
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
                spinBox.realValue = dialogModel.scaleValue;
            }
        }
        D.Button {
            id: scaleButton
            icon.source: "qrc:/general/icons/true.svg"
            text: "Scale"

            onClicked: {
                dialogModel.setScaleType(dialogModel.Scale);
                dialogModel.updateScaleValue(spinBox.realValue);
                dialogModel.acceptSelection();
            }
        }
        D.Button {
            id: scaleToButton
            icon.source: "qrc:/general/icons/true.svg"
            text: "Scale To"

            onClicked: {
                dialogModel.setScaleType(dialogModel.ScaleTo);
                dialogModel.updateScaleValue(spinBox.realValue);
                dialogModel.acceptSelection();
            }
        }
    }
}
