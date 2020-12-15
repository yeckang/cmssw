#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Validation/MuonGEMDigis/plugins/GEMStripDigiValidation.h"

GEMStripDigiValidation::GEMStripDigiValidation(const edm::ParameterSet& pset)
    : GEMBaseValidation(pset, "GEMStripDigiValidation") {
  const auto& strip_pset = pset.getParameterSet("gemStripDigi");
  const auto& strip_tag = strip_pset.getParameter<edm::InputTag>("inputTag");
  strip_token_ = consumes<GEMDigiCollection>(strip_tag);

  const auto& simhit_pset = pset.getParameterSet("gemSimHit");
  const auto& simhit_tag = simhit_pset.getParameter<edm::InputTag>("inputTag");
  simhit_token_ = consumes<edm::PSimHitContainer>(simhit_tag);

  const auto& digisimlink_pset = pset.getParameterSet("gemDigiSimLink");
  const auto& digisimlink_tag = digisimlink_pset.getParameter<edm::InputTag>("inputTag");
  digisimlink_token_ = consumes<edm::DetSetVector<GEMDigiSimLink>>(digisimlink_tag);

  geomToken_ = esConsumes<GEMGeometry, MuonGeometryRecord>();
  geomTokenBeginRun_ = esConsumes<GEMGeometry, MuonGeometryRecord, edm::Transition::BeginRun>();
}

void GEMStripDigiValidation::bookHistograms(DQMStore::IBooker& booker,
                                            edm::Run const& run,
                                            edm::EventSetup const& setup) {
  const GEMGeometry* gem = &setup.getData(geomTokenBeginRun_);
  if (gem == nullptr) {
    edm::LogError(kLogCategory_) << "Failed to initialize GEM geometry.";
    return;
  }

  // NOTE Bunch Crossing
  booker.setCurrentFolder("MuonGEMDigisV/GEMDigisTask/Strip/BunchCrossing");

  me_bx_ = booker.book1D("strip_bx", "Strip Digi Bunch Crossing", 11, -10.5, 10.5);

  if (detail_plot_) {
    for (const auto& region : gem->regions()) {
      if (region == nullptr) {
        edm::LogError(kLogCategory_) << "Null region";
        continue;
      }
      Int_t region_id = region->region();
      for (const auto& station : region->stations()) {
        if (station == nullptr) {
          edm::LogError(kLogCategory_) << "Null station for region = " << region_id;
          continue;
        }
        Int_t station_id = station->station();

        const auto& superChamberVec = station->superChambers();
        if (superChamberVec.empty()) {
          edm::LogError(kLogCategory_) << "Super chambers missing for region = " << region_id
                                       << " and station = " << station_id;
          continue;
        }
        const GEMSuperChamber* super_chamber = superChamberVec.front();
        if (super_chamber == nullptr) {
          edm::LogError(kLogCategory_) << "Failed to find super chamber for region = " << region_id
                                       << " and station = " << station_id;
          continue;
        }
        for (const auto& chamber : super_chamber->chambers()) {
          Int_t layer_id = chamber->id().layer();
          ME3IdsKey key3(region_id, station_id, layer_id);

          me_detail_bx_[key3] =
              bookHist1D(booker, key3, "strip_bx", "Strip Digi Bunch Crossing", 11, -10.5, 10.5, "Bunch crossing");
        }  // chamber loop
      }    // station loop
    }      // region loop
  }        // detail plot

  // NOTE Occupancy
  booker.setCurrentFolder("MuonGEMDigisV/GEMDigisTask/Strip/Occupancy");

  me_total_strip_ = booker.book1D("total_strips_per_event", "Total number of Strip Digi per Event", 50, -0.5, 395.5);

  for (const auto& region : gem->regions()) {
    Int_t region_id = region->region();

    if (detail_plot_)
      me_detail_occ_zr_[region_id] = bookZROccupancy(booker, region_id, "strip", "Strip Digi");

    for (const auto& station : region->stations()) {
      Int_t station_id = station->station();
      ME2IdsKey key2{region_id, station_id};

      if (detail_plot_) {
        me_detail_strip_occ_det_[key2] =
            bookDetectorOccupancy(booker, key2, station, "matched_strip", "Matched Strip Digi");

        me_detail_occ_det_[key2] = bookDetectorOccupancy(booker, key2, station, "strip", "Strip Digi");
      }

      const auto& superChamberVec = station->superChambers();
      if (superChamberVec.empty() || superChamberVec[0] == nullptr) {
        edm::LogError(kLogCategory_) << "Super chambers missing or null for region = " << region_id
                                     << " and station = " << station_id;
      } else {
        for (const auto& chamber : superChamberVec[0]->chambers()) {
          if (chamber == nullptr) {
            edm::LogError(kLogCategory_) << "Null chamber for region, station, super chamber = (" << region_id << ", "
                                         << station_id << ", " << superChamberVec[0]->id() << ")";
            continue;
          }
          Int_t layer_id = chamber->id().layer();
          ME3IdsKey key3{region_id, station_id, layer_id};

          const auto& etaPartitionsVec = chamber->etaPartitions();
          if (etaPartitionsVec.empty() || etaPartitionsVec.front() == nullptr) {
            edm::LogError(kLogCategory_)
                << "Eta partition missing or null for region, station, super chamber, chamber = (" << region_id << ", "
                << station_id << ", " << superChamberVec[0]->id() << ", " << chamber->id() << ")";
            continue;
          }
          Int_t num_strips = etaPartitionsVec.front()->nstrips();
          Int_t num_eta_partitions = chamber->nEtaPartitions();

          me_occ_ieta_[key3] = bookHist1D(booker,
                                          key3,
                                          "strip_ieta_occ",
                                          "Strip Digi Occupancy per eta partition",
                                          num_eta_partitions,
                                          0.5,
                                          num_eta_partitions + 0.5,
                                          "i_{#eta}");

          me_occ_phi_[key3] =
              bookHist1D(booker, key3, "strip_phi_occ", "Strip Digi Phi Occupancy", 108, -5, 355, "#phi [degrees]");

          me_total_strip_layer_[key3] = bookHist1D(booker,
                                                   key3,
                                                   "total_strips_per_event",
                                                   "Total nubmer of strip digis per event for each layers",
                                                   50,
                                                   -0.5,
                                                   99.5);

          // occupancy plots for efficiency
          me_strip_occ_eta_[key3] = bookHist1D(booker,
                                               key3,
                                               "matched_strip_occ_eta",
                                               "Matched Strip Digi Eta Occupancy",
                                               16,
                                               eta_range_[station_id * 2 + 0],
                                               eta_range_[station_id * 2 + 1],
                                               "|#eta|");

          me_strip_occ_phi_[key3] = bookHist1D(
              booker, key3, "matched_strip_occ_phi", "Matched Strip Digi Phi Occupancy", 36, -5, 355, "#phi");

          if (detail_plot_) {
            me_detail_occ_xy_[key3] = bookXYOccupancy(booker, key3, "strip", "Strip Digi");

            me_detail_occ_strip_[key3] = bookHist1D(booker,
                                                    key3,
                                                    "strip_occ_strip",
                                                    "Strip Digi Occupancy per strip number",
                                                    num_strips,
                                                    0.5,
                                                    num_strips + 0.5,
                                                    "strip number");
          }  // detail plot
        }    // chamber
      }      // end else
    }        // station looop
  }          // region loop
}

GEMStripDigiValidation::~GEMStripDigiValidation() {}

void GEMStripDigiValidation::analyze(const edm::Event& event, const edm::EventSetup& setup) {
  const GEMGeometry* gem = &setup.getData(geomToken_);
  if (gem == nullptr) {
    edm::LogError(kLogCategory_) << "Failed to initialize GEM geometry.";
    return;
  }
  edm::Handle<edm::DetSetVector<GEMDigiSimLink>> digiSimLink;
  event.getByToken(digisimlink_token_, digiSimLink);
  if (not digiSimLink.isValid()) {
    edm::LogError(kLogCategory_) << "Failed to get GEMDigiSimLink." << std::endl;
    return;
  }

  edm::Handle<edm::PSimHitContainer> simhit_container;
  event.getByToken(simhit_token_, simhit_container);
  if (not simhit_container.isValid()) {
    edm::LogError(kLogCategory_) << "Failed to get PSimHitContainer." << std::endl;
    return;
  }

  edm::Handle<GEMDigiCollection> digi_collection;
  event.getByToken(strip_token_, digi_collection);
  if (not digi_collection.isValid()) {
    edm::LogError(kLogCategory_) << "Cannot get strips by Token stripToken." << std::endl;
    return;
  }

  // NOTE
  Int_t total_strip = 0;
  std::map<ME3IdsKey, Int_t> total_strip_layer;
  for (const auto& etaPart : *digi_collection) {
    GEMDetId id = etaPart.first;
    if (gem->idToDet(id) == nullptr) {
      edm::LogError(kLogCategory_) << "Getting DetId failed. Discard this gem strip hit. Maybe it comes "
                                   << "from unmatched geometry." << std::endl;
      continue;
    }

    Int_t region_id = id.region();
    Int_t layer_id = id.layer();
    Int_t station_id = id.station();
    Int_t chamber_id = id.chamber();
    Int_t roll_id = id.roll();
    Int_t num_chambers = id.nlayers();

    ME2IdsKey key2{region_id, station_id};
    ME3IdsKey key3{region_id, station_id, layer_id};
    Int_t bin_x = getDetOccBinX(num_chambers, chamber_id, layer_id);

    const BoundPlane& surface = gem->idToDet(id)->surface();
    const GEMEtaPartition* roll = gem->etaPartition(id);

    const GEMDigiCollection::Range& range = etaPart.second;
    for (auto digi = range.first; digi != range.second; ++digi) {
      total_strip++;
      total_strip_layer[key3]++;
      Int_t strip = digi->strip();
      Int_t bx = digi->bx();
      // bunch corssing overflow & underflow
      bx = bx < -10 ? -10 : bx;
      bx = bx > 10 ? 10 : bx;

      GlobalPoint strip_global_pos = surface.toGlobal(roll->centreOfStrip(digi->strip()));

      Float_t digi_g_r = strip_global_pos.perp();
      Float_t digi_g_abs_z = std::abs(strip_global_pos.z());
      Float_t digi_g_x = strip_global_pos.x();
      Float_t digi_g_y = strip_global_pos.y();
      Float_t digi_g_phi = toDegree(strip_global_pos.phi());

      me_bx_->Fill(bx);
      me_occ_ieta_[key3]->Fill(roll_id);
      me_occ_phi_[key3]->Fill(digi_g_phi);

      if (detail_plot_) {
        me_detail_bx_[key3]->Fill(bx);
        me_detail_occ_xy_[key3]->Fill(digi_g_x, digi_g_y);
        me_detail_occ_zr_[region_id]->Fill(digi_g_abs_z, digi_g_r);
        me_detail_occ_det_[key2]->Fill(bin_x, roll_id);
        me_detail_occ_strip_[key3]->Fill(strip);
      }
    }
  }  // range loop
  me_total_strip_->Fill(total_strip);
  for (auto [key, num_total_strip] : total_strip_layer)
    me_total_strip_layer_[key]->Fill(num_total_strip);

  // NOTE
  for (const auto& simhit : *simhit_container.product()) {
    if (not isMuonSimHit(simhit))
      continue;

    if (gem->idToDet(simhit.detUnitId()) == nullptr) {
      edm::LogError(kLogCategory_) << "SimHit did not match with GEMGeometry." << std::endl;
      continue;
    }

    GEMDetId simhit_gemid(simhit.detUnitId());

    Int_t region_id = simhit_gemid.region();
    Int_t station_id = simhit_gemid.station();
    Int_t layer_id = simhit_gemid.layer();
    Int_t chamber_id = simhit_gemid.chamber();
    Int_t roll_id = simhit_gemid.roll();
    Int_t num_chambers = simhit_gemid.nlayers();

    ME2IdsKey key2{region_id, station_id};
    ME3IdsKey key3{region_id, station_id, layer_id};

    const GEMEtaPartition* roll = gem->etaPartition(simhit_gemid);

    const auto& simhit_local_pos = simhit.localPosition();
    const auto& simhit_global_pos = roll->surface().toGlobal(simhit_local_pos);

    Float_t simhit_g_eta = std::abs(simhit_global_pos.eta());
    Float_t simhit_g_phi = toDegree(simhit_global_pos.phi());

    auto simhit_trackId = simhit.trackId();

    Int_t bin_x = getDetOccBinX(num_chambers, chamber_id, layer_id);

    auto links = digiSimLink->find(simhit_gemid);
    if (links == digiSimLink->end())
      continue;

    for (const auto& link : *links) {
      if (simhit_trackId == link.getTrackId()) {
        me_strip_occ_eta_[key3]->Fill(simhit_g_eta);
        me_strip_occ_phi_[key3]->Fill(simhit_g_phi);
        if (detail_plot_)
          me_detail_strip_occ_det_[key2]->Fill(bin_x, roll_id);
        break;
      }
    }
  }  // simhit_container loop
}
