#ifndef CondFormats_GEMObjects_GEMeMap_h
#define CondFormats_GEMObjects_GEMeMap_h

#include "CondFormats/Serialization/interface/Serializable.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include <map>
#include <string>
#include <vector>
#include <algorithm>

class GEMeMap {
public:
  struct sectorEC {
    unsigned int fedId;
    uint8_t amcNum;
    bool operator==(const sectorEC& r) const {
      if (fedId == r.fedId) {
        return amcNum == r.amcNum;
      } else {
        return false;
      }
    }

    COND_SERIALIZABLE;
  };

  struct chamEC {
    unsigned int fedId;
    uint8_t amcNum;
    uint16_t gebId;
    bool operator<(const chamEC& r) const {
      if (fedId == r.fedId) {
        if (amcNum == r.amcNum) {
          return gebId < r.gebId;
        } else {
          return amcNum < r.amcNum;
        }
      } else {
        return fedId < r.fedId;
      }
    }

    COND_SERIALIZABLE;
  };

  struct chamDC {
    uint32_t detId;
    int chamberType;
    int vfatVer;
    bool operator<(const chamDC& r) const { return detId < r.detId; }

    COND_SERIALIZABLE;
  };

  struct vfatEC {
    int chamberType;
    uint16_t vfatAdd;
    bool operator<(const vfatEC& r) const {
      if (vfatAdd == r.vfatAdd) {
        return chamberType < r.chamberType;
      } else {
        return vfatAdd < r.vfatAdd;
      }
    }

    COND_SERIALIZABLE;
  };

  struct channelNum {
    int chamberType;
    int vfatAdd;
    int chNum;
    bool operator<(const channelNum& c) const {
      if (chamberType == c.chamberType) {
        if (vfatAdd == c.vfatAdd) {
          return chNum < c.chNum;
        } else {
          return vfatAdd < c.vfatAdd;
        }
      } else {
        return chamberType < c.chamberType;
      }
    }

    COND_SERIALIZABLE;
  };

  struct stripNum {
    int chamberType;
    int iEta;
    int stNum;
    bool operator<(const stripNum& s) const {
      if (chamberType == s.chamberType) {
        if (iEta == s.iEta) {
          return stNum < s.stNum;
        } else {
          return iEta < s.iEta;
        }
      } else {
        return chamberType < s.chamberType;
      }
    }

    COND_SERIALIZABLE;
  };

  GEMeMap();

  explicit GEMeMap(const std::string& version);

  ~GEMeMap();

  const std::string& version() const;
  void setDummy();

  std::map<chamEC, chamDC> chamberMap() { return chamberMap_; };

  bool isValidChamber(const chamEC& r) const { return chamberMap_.find(r) != chamberMap_.end(); }

  bool isValidChipID(const vfatEC& r) const { return chamIEtas_.find(r) != chamIEtas_.end(); }

  bool isValidAMC(const sectorEC& r) const { return std::find(amcVec_.begin(), amcVec_.end(), r) != amcVec_.end(); }

  bool isValidStripNum(const stripNum& r) const { return stChMap_.find(r) != stChMap_.end(); }

  void add(sectorEC e) { amcVec_.push_back(e); }

  const chamDC& chamberPos(const chamEC& r) const { return chamberMap_.at(r); }
  void add(chamEC e, chamDC d) { chamberMap_[e] = d; }

  const std::vector<uint16_t> getVfats(const int type) const { return chamVfats_.at(type); }
  void add(int type, uint16_t d) { chamVfats_[type].push_back(d); }

  const std::vector<int> getIEtas(const vfatEC dc) const { return chamIEtas_.at(dc); }
  void add(vfatEC d, int iEta) { chamIEtas_[d].push_back(iEta); }

  const channelNum& hitPos(const stripNum& s) const { return stChMap_.at(s); }
  const stripNum& hitPos(const channelNum& c) const { return chStMap_.at(c); }

  void add(channelNum c, stripNum s) { chStMap_[c] = s; }
  void add(stripNum s, channelNum c) { stChMap_[s] = c; }

private:
  std::string theVersion;

  std::vector<sectorEC> amcVec_;

  // electronics map to GEMDetId chamber
  std::map<chamEC, chamDC> chamberMap_;

  std::map<int, std::vector<uint16_t>> chamVfats_;
  std::map<vfatEC, std::vector<int>> chamIEtas_;

  std::map<channelNum, stripNum> chStMap_;
  std::map<stripNum, channelNum> stChMap_;

  COND_SERIALIZABLE;

public:
  // size of ID bits
  static const int vfatVerV3_ = 3;       // VFAT v3
  static const int vfatTypeV3_ = 11;     // VFAT v3
  static const int chipIdMask_ = 0xfff;  // chipId mask for 12 bits
  static const int maxGEBs_ = 24;        // 5 bits for GEB id
  static const int maxGEB1_ = 12;        // 5 bits for GEB id
  static const int maxGEB2_ = 12;        // 5 bits for GEB id
  static const int maxAMCs_ = 15;        // 4 bits for AMC no.
  static const int maxVFatGE0_ = 12;     // vFat per eta partition, not known yet for ME0
  static const int maxVFatGE11_ = 3;     // vFat per eta partition in GE11
  static const int maxVFatGE21_ = 6;     // vFat per eta partition in GE21
  static const int maxiEtaIdGE0_ = 8;    // no. eta partitions for GE0
  static const int maxiEtaIdGE11_ = 8;   // no. eta partitions for GE11
  static const int maxiEtaIdGE21_ = 16;  // no. eta partitions for GE21
  static const int maxChan_ = 128;       // channels per vFat
};
#endif  // GEMeMap_H
