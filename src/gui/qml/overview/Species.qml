import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve 1.0

Node {
    property variant model
    property variant modelData

    function generateSpeciesMarkup(index) {
        const speciesData = model.data;
        const speciesIndex = model.index(index, 0);
        const speciesName = speciesData(speciesIndex, Qt.DisplayRole);
        const bonds = speciesData(speciesIndex, SpeciesModel.SpeciesUserRole.BondsCount);
        const angles = speciesData(speciesIndex, SpeciesModel.SpeciesUserRole.AnglesCount);
        const torsions = speciesData(speciesIndex, SpeciesModel.SpeciesUserRole.TorsionsCount);
        const impropers = speciesData(speciesIndex, SpeciesModel.SpeciesUserRole.ImpropersCount);
        const markup = `
        <div style="font-size: 16px;">
            ${speciesName} <img src="qrc:/general/icons/species.svg" width="15" height="15" alt=""> <br/>
        </div>
        <div style="font-size: 14px; text-align: center;">
            Bonds: ${bonds}<br/>
            Angles: ${angles}<br/>
            Torsions: ${torsions}<br/>
            Impropers: ${impropers}
        </div>
    `;
        return markup;
    }

    Text {
        text: generateSpeciesMarkup(index)
    }
}
