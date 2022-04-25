#include "CondFormats/Serialization/interface/Test.h"
#include "CondFormats/GEMObjects/src/headers.h"

int main() {
  testSerialization<GEMELMap>();
  testSerialization<GEMELMap::GEMVFatMap>();
  testSerialization<std::vector<GEMELMap::GEMVFatMap>>();
  testSerialization<GEMELMap::GEMStripMap>();
  testSerialization<std::vector<GEMELMap::GEMStripMap>>();

  testSerialization<GEMeMap>();
  testSerialization<GEMeMap::GEMChamberMap>();
  testSerialization<std::vector<GEMeMap::GEMChamberMap>>();
  testSerialization<GEMeMap::GEMVFatMap>();
  testSerialization<std::vector<GEMeMap::GEMVFatMap>>();
  testSerialization<GEMeMap::GEMStripMap>();
  testSerialization<std::vector<GEMeMap::GEMStripMap>>();

  testSerialization<GEMeMapping>();
  testSerialization<GEMeMapping::sectorEC>();
  testSerialization<GEMeMapping::chamEC>();
  testSerialization<GEMeMapping::chamDC>();
  testSerialization<GEMeMapping::vfatEC>();
  testSerialization<GEMeMapping::channelNum>();
  testSerialization<GEMeMapping::stripNum>();
  testSerialization<std::vector<GEMeMapping::sectorEC>>();
  testSerialization<std::map<GEMeMapping::chamEC, GEMeMapping::chamDC>>();
  testSerialization<std::map<int, std::vector<uint16_t>>>();
  testSerialization<std::map<GEMeMapping::vfatEC, std::vector<int>>>();
  testSerialization<std::map<GEMeMapping::channelNum, GEMeMapping::stripNum>>();
  testSerialization<std::map<GEMeMapping::stripNum, GEMeMapping::channelNum>>();

  testSerialization<GEMDeadStrips>();
  testSerialization<GEMDeadStrips::DeadItem>();
  testSerialization<GEMMaskedStrips>();
  testSerialization<GEMMaskedStrips::MaskItem>();
}
