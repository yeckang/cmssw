#ifndef Validation_MuonGEMHits_GEMSimHitValidation_h
#define Validation_MuonGEMHits_GEMSimHitValidation_h

#include "Validation/MuonGEMHits/interface/GEMBaseValidation.h"

#include <tuple>
#include <map>
#include <vector>
#include <string>

class GEMSimHitValidation : public GEMBaseValidation {
public:
  explicit GEMSimHitValidation(const edm::ParameterSet&);
  ~GEMSimHitValidation() override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;

private:
  std::tuple<Double_t, Double_t> getTOFRange(Int_t station_id);

  // Parameters
  edm::EDGetTokenT<edm::PSimHitContainer> simhit_token_;
  edm::ESGetToken<GEMGeometry, MuonGeometryRecord> geomToken_;
  edm::ESGetToken<GEMGeometry, MuonGeometryRecord> geomTokenBeginRun_;
  std::vector<Double_t> tof_range_;

  // Monitor elemnts
  std::map<Int_t, MonitorElement*> me_tof_mu_st_;  // time of flight
  MonitorElement* me_total_hits_;
  MEMap3Ids me_tof_;
  MEMap3Ids me_tof_mu_;
  MEMap1Ids me_eloss_mu_;  // energy loss
  MEMap3Ids me_occ_ieta_;
  MEMap3Ids me_occ_phi_;
  MEMap3Ids me_mu_occ_eta_;
  MEMap3Ids me_mu_occ_phi_;

  MEMap3Ids me_detail_eloss_;
  MEMap3Ids me_detail_eloss_mu_;
  MEMap1Ids me_detail_occ_zr_;  // occupancy
  MEMap3Ids me_detail_occ_xy_;
  MEMap2Ids me_detail_occ_det_;
  MEMap2Ids me_detail_mu_occ_det_;

  // Constants
  const Float_t kEnergyCF_ = 1e6f;  // energy loss conversion factor:
};

#endif  // Validation_MuonGEMHits_GEMSimHitValidation_h
