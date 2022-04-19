#include "CondFormats/Serialization/interface/Test.h"
#include "CondFormats/GEMObjects/src/headers.h"

int main() {
  testSerialization<GEMELMap>();
  testSerialization<GEMELMap::GEMChamberMap>();
  testSerialization<std::vector<GEMELMap::GEMChamberMap>>();
  testSerialization<GEMELMap::GEMStripMap>();
  testSerialization<std::vector<GEMELMap::GEMStripMap>>();

  testSerialization<GEMeMap>();
  testSerialization<GEMeMap::sectorEC>();
  testSerialization<GEMeMap::chamEC>();
  testSerialization<GEMeMap::chamDC>();
  testSerialization<GEMeMap::vfatEC>();
  testSerialization<GEMeMap::channelNum>();
  testSerialization<GEMeMap::stripNum>();
  testSerialization<std::vector<GEMeMap::sectorEC>>();
  testSerialization<std::map<GEMeMap::chamEC, GEMeMap::chamDC>>();
  testSerialization<std::map<int, std::vector<uint16_t>>>();
  testSerialization<std::map<GEMeMap::vfatEC, std::vector<int>>>();
  testSerialization<std::map<GEMeMap::channelNum, GEMeMap::stripNum>>();
  testSerialization<std::map<GEMeMap::stripNum, GEMeMap::channelNum>>();

  testSerialization<GEMDeadStrips>();
  testSerialization<GEMDeadStrips::DeadItem>();
  testSerialization<GEMMaskedStrips>();
  testSerialization<GEMMaskedStrips::MaskItem>();
}
