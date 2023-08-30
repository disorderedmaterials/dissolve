import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0

Node {
    property variant model
    property variant modelData

    function generateConfigurationMarkup(index) {
        const configurationData = model.data;
        const configurationIndex = model.index(index, 0);
        const configurationName = configurationData(configurationIndex, Qt.DisplayRole);
        const molecules = configurationData(configurationIndex, ConfigurationModel.ConfigurationUserRole.MoleculesCount);
        const atoms = configurationData(configurationIndex, ConfigurationModel.ConfigurationUserRole.AtomsCount);
        const markup = `
        <div style="font-size: 16px;">
            ${configurationName} <img src="qrc:/general/icons/configuration.svg" width="15" height="15"> <br/>
        </div>
        <div style="font-size: 14px; text-align: center;">
            Molecules: ${molecules} <br/>
            Atoms: ${atoms}
        </div>
    `;
        return markup;
    }

    Text {
        text: generateConfigurationMarkup(index)
    }
}
