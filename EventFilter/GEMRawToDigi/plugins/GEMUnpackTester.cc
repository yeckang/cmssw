#include <memory>
#include <iostream>
#include <TH2D.h>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"
#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"

using namespace std;
typedef std::tuple<int, int, int, int> Key4;
struct Empty {};
class GEMUnpackTester : public edm::one::EDAnalyzer<edm::one::WatchRuns,edm::one::SharedResources> {
public:
  explicit GEMUnpackTester(const edm::ParameterSet&);
  ~GEMUnpackTester() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void beginRun(edm::Run const&, edm::EventSetup const&) override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endRun(edm::Run const&, edm::EventSetup const&) override;
  void endJob() override;

  edm::EDGetTokenT<GEMDigiCollection> gemDigiToken_;
  edm::EDGetTokenT<GEMDigiCollection> gemSimDigiToken_;
  const edm::ESGetToken<GEMGeometry, MuonGeometryRecord> geoToken_;

  std::map<Key4, TH2D*> digi_occ_;
  std::map<Key4, TH2D*> sim_occ_;
};

GEMUnpackTester::GEMUnpackTester(const edm::ParameterSet& iConfig)
    : gemDigiToken_(consumes<GEMDigiCollection>(iConfig.getParameter<edm::InputTag>("gemDigi"))),
      gemSimDigiToken_(consumes<GEMDigiCollection>(iConfig.getParameter<edm::InputTag>("gemSimDigi"))),
      geoToken_{esConsumes<GEMGeometry, MuonGeometryRecord, edm::Transition::BeginRun>(edm::ESInputTag{})} {
  usesResource("TFileService");
  edm::Service<TFileService> fs;
}

GEMUnpackTester::~GEMUnpackTester() {}

void GEMUnpackTester::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  auto const& gemDigis = iEvent.get(gemDigiToken_);
  auto const& gemSimDigis = iEvent.get(gemSimDigiToken_);

  for (auto const& simDigi : gemSimDigis) {
    const GEMDetId& gemId = simDigi.first;
    const GEMDigiCollection::Range& sim = simDigi.second;
    const GEMDigiCollection::Range& packed = gemDigis.get(gemId);

    int re = gemId.region();
    int st = gemId.station();
    int ch = gemId.chamber();
    int la = gemId.layer();
    int ieta = gemId.ieta();
    Key4 key{re, st, ch, la};

    for (auto digi = sim.first; digi != sim.second; ++digi) {
      int strip = digi->strip();
      sim_occ_[key]->Fill(strip, ieta);
    }
    for (auto digi = packed.first; digi != packed.second; ++digi) {
      int strip = digi->strip();
      digi_occ_[key]->Fill(strip, ieta);
    }
  }
}

void GEMUnpackTester::beginJob() {}

void GEMUnpackTester::endJob() {}

void GEMUnpackTester::beginRun(edm::Run const&, edm::EventSetup const& iSetup) {
  const auto& pDD = iSetup.getData(geoToken_);
  edm::Service<TFileService> fs;

  //tree_ = fs->make<TTree>("fed", "fed");
  for (auto chamber : pDD.chambers()) {
    auto cid = chamber->id();
    int nEtaPart = chamber->nEtaPartitions();

    int re = cid.region();
    int st = cid.station();
    int ch = cid.chamber();
    int la = cid.layer();
    Key4 key{re, st, ch, la};
    
    auto name_digi = Form("digi_occ_re%d_st%d_ch%d_la%d", re, st, ch, la);
    auto title_digi = Form("Re %d St %d, Ch %d La %d;strip;iEta", re, st, ch, la);
    digi_occ_[key] = fs->make<TH2D>(name_digi, 
                                    title_digi, 
                                    384, -0.5, 383.5, 
                                    nEtaPart, 0.5, 0.5+nEtaPart);
    auto name_sim = Form("sim_occ_re%d_st%d_ch%d_la%d", re, st, ch, la);
    auto title_sim = Form("Re %d St %d, Ch %d La %d;strip;iEta", re, st, ch, la);
    sim_occ_[key] = fs->make<TH2D>(name_sim, 
                                   title_sim, 
                                   384, -0.5, 383.5, 
                                   nEtaPart, 0.5, 0.5+nEtaPart);
  }
}

void GEMUnpackTester::endRun(edm::Run const&, edm::EventSetup const&) {}

void GEMUnpackTester::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("gemDigi", edm::InputTag("muonGEMDigis"));
  desc.add<edm::InputTag>("gemSimDigi", edm::InputTag("simMuonGEMDigis"));
  descriptions.add("GEMUnpackTester", desc);
}

DEFINE_FWK_MODULE(GEMUnpackTester);
