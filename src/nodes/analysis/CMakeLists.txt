add_library(
  nodes_analysis
  angle.cpp
  angle.h
  averageMolecule.cpp
  averageMolecule.h
  axisAngle.cpp
  axisAngle.h
  dAngle.cpp
  dAngle.h
  histogramCN.cpp
  histogramCN.h
  intraAngle.cpp
  intraAngle.h
  intraDistance.cpp
  intraDistance.h
  modifierOSites.cpp
  modifierOSites.h
  moleculeTorsion.cpp
  moleculeTorsion.h
  orientedSDF.cpp
  orientedSDF.h
  qSpecies.cpp
  qSpecies.h
  sdf.cpp
  sdf.h
  siteRDF.cpp
  siteRDF.h
  voxelDensity.cpp
  voxelDensity.h
)

target_include_directories(nodes_analysis PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(nodes_analysis PUBLIC base ${THREADING_LINK_LIBS})
