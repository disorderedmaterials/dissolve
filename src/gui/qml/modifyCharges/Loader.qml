import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve

Page {
    id: root
    height: 166
    title: {
        switch(modify) {
            case ModifyChargesModel.Scaling:
                return "Scale Charges";
            case ModifyChargesModel.Smoothing:
                return "Smooth Charges";
            case ModifyChargesModel.ReduceSigFig:
                return "Reduce Significant Figures";
            default:
                break;
        }
    }
    width: 400
    visible: true

    property double scaleValue
    property double smoothValue
    property double sigFigValue

    Item {
        anchors.fill: parent
        anchors.margin: 10

        Loader {
            id: loader
            anchors.fill: parent
            sourceComponent: {
                switch(modify) {
                    case ModifyChargesModel.Scaling:
                        loader.setSource("", {
                            "dialogModel": dialogModel
                            "modify": modify
                            "spinBoxValue": scaleValue
                        })
                        break
                    case ModifyChargesModel.Smoothing:
                        loader.setSource("", {
                            "dialogModel": dialogModel
                            "modify": modify
                            "spinBoxValue": smoothValue
                        })
                        break
                    case ModifyChargesModel.ReduceSigFig:
                        loader.setSource("", {
                            "dialogModel": dialogModel
                            "modify": modify
                            "spinBoxValue": smoothValue
                        })
                        break
                }
            }
        }
    }

}