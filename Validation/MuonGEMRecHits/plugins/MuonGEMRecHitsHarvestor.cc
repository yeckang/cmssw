#include "Validation/MuonGEMRecHits/plugins/MuonGEMRecHitsHarvestor.h"
#include "Validation/MuonGEMHits/interface/GEMValidationUtils.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

MuonGEMRecHitsHarvestor::MuonGEMRecHitsHarvestor(const edm::ParameterSet& pset)
    : MuonGEMBaseHarvestor(pset, "MuonGEMRecHitsHarvestor") {
  region_ids_ = pset.getUntrackedParameter<std::vector<Int_t> >("regionIds");
  station_ids_ = pset.getUntrackedParameter<std::vector<Int_t> >("stationIds");
  layer_ids_ = pset.getUntrackedParameter<std::vector<Int_t> >("layerIds");
}

MuonGEMRecHitsHarvestor::~MuonGEMRecHitsHarvestor() {}

void MuonGEMRecHitsHarvestor::dqmEndJob(DQMStore::IBooker& booker, DQMStore::IGetter& getter) {
  TString simhit_occ_folder = "MuonGEMHitsV/GEMHitsTask/Occupancy/";
  TString occ_folder = "MuonGEMRecHitsV/GEMRecHitsTask/Occupancy/";
  TString eff_folder = "MuonGEMRecHitsV/GEMRecHitsTask/Efficiency/";
  TString residual_folder = "MuonGEMRecHitsV/GEMRecHitsTask/Residual/";
  TString pull_folder = "MuonGEMRecHitsV/GEMRecHitsTask/Pull/";
  TString cls_folder = "MuonGEMRecHitsV/GEMRecHitsTask/ClusterSize/";
  TString summary_folder = "MuonGEMRecHitsV/GEMRecHitsTask/Summary/";

  for (const auto& region_id : region_ids_) {
    for (const auto& station_id : station_ids_) {
      TString name_suf_re_st = GEMUtils::getSuffixName(region_id, station_id);
      TString title_suf_re_st = GEMUtils::getSuffixTitle(region_id, station_id);

      if (detail_plot_) {
        // NOTE Detector Component
        TString rechit_det_path = occ_folder + "matched_rechit_occ_det" + name_suf_re_st;
        TString simhit_det_path = simhit_occ_folder + "muon_simhit_occ_det" + name_suf_re_st;

        TString eff_det_name = "eff_det" + name_suf_re_st;
        TString eff_det_title = "Detector Component Efficiency :" + title_suf_re_st;

        bookEff2D(booker, getter, rechit_det_path, simhit_det_path, eff_folder, eff_det_name, eff_det_title);
      }
      for (const auto& layer_id : layer_ids_) {
        if (station_id != 0 and layer_id > 2) continue;
        TString name_suf_re_st_ly = GEMUtils::getSuffixName(region_id, station_id, layer_id);
        TString title_suf_re_st_ly = GEMUtils::getSuffixTitle(region_id, station_id, layer_id);

        // NOTE Eta
        TString rechit_eta_path = occ_folder + "matched_rechit_occ_eta" + name_suf_re_st_ly;
        TString simhit_eta_path = simhit_occ_folder + "muon_simhit_occ_eta" + name_suf_re_st_ly;

        TString eff_eta_name = "eff_eta" + name_suf_re_st_ly;
        TString eff_eta_title = "Eta Efficiency :" + title_suf_re_st_ly;

        bookEff1D(booker, getter, rechit_eta_path, simhit_eta_path, eff_folder, eff_eta_name, eff_eta_title);

        // NOTE Phi
        TString rechit_phi_path = occ_folder + "matched_rechit_occ_phi" + name_suf_re_st_ly;
        TString simhit_phi_path = simhit_occ_folder + "muon_simhit_occ_phi" + name_suf_re_st_ly;

        TString eff_phi_name = "eff_phi" + name_suf_re_st_ly;
        TString eff_phi_title = "Phi Efficiency :" + title_suf_re_st_ly;

        bookEff1D(booker, getter, rechit_phi_path, simhit_phi_path, eff_folder, eff_phi_name, eff_phi_title);

        if (detail_plot_) {
          // NOTE Residual
          TString residual_x_path = residual_folder + "residual_x" + name_suf_re_st_ly;
          TString mean_res_x_name = "residual_x" + name_suf_re_st_ly;
          TString mean_res_x_title = "Residual X :" + title_suf_re_st_ly;

          bookMean1D(booker, getter, residual_x_path, summary_folder, mean_res_x_name, mean_res_x_title);

          TString residual_y_path = residual_folder + "residual_y" + name_suf_re_st_ly;
          TString mean_res_y_name = "residual_y" + name_suf_re_st_ly;
          TString mean_res_y_title = "Residual Y :" + title_suf_re_st_ly;

          bookMean1D(booker, getter, residual_y_path, summary_folder, mean_res_y_name, mean_res_y_title);

          // NOTE Pull
          TString pull_x_path = pull_folder + "pull_x" + name_suf_re_st_ly;
          TString mean_pul_x_name = "pull_x" + name_suf_re_st_ly;
          TString mean_pul_x_title = "Pull X :" + title_suf_re_st_ly;

          bookMean1D(booker, getter, pull_x_path, summary_folder, mean_pul_x_name, mean_pul_x_title);

          TString pull_y_path = pull_folder + "pull_y" + name_suf_re_st_ly;
          TString mean_pul_y_name = "pull_y" + name_suf_re_st_ly;
          TString mean_pul_y_title = "Pull Y :" + title_suf_re_st_ly;

          bookMean1D(booker, getter, pull_y_path, summary_folder, mean_pul_y_name, mean_pul_y_title);

          // Note Cluster Size
          TString cls_path = cls_folder + "cls" + name_suf_re_st_ly;
          TString mean_cls_name = "cls" + name_suf_re_st_ly;
          TString mean_cls_title = "Cluster Size :" + title_suf_re_st_ly;

          bookMean1D(booker, getter, cls_path, summary_folder, mean_cls_name, mean_cls_title);
        }
      }  // layer loop
    }    // station loop
  }      // region loop
}

void MuonGEMRecHitsHarvestor::bookMean1D(DQMStore::IBooker& booker,
                                         DQMStore::IGetter& getter,
                                         const TString residual_path,
                                         const TString folder,
                                         const TString name,
                                         const TString title) {
  Int_t num_eta_partitions = 8;
  if (getElement<TH1F>(getter, residual_path + "_R1") == nullptr) {
    edm::LogError(kLogCategory_) << "failed to get plots from " << residual_path << std::endl;
    return;
  }
  if (getElement<TH1F>(getter, residual_path + "_R16") != nullptr)
    num_eta_partitions = 16;
  TProfile* profile = new TProfile(name, title, num_eta_partitions, 0.5, num_eta_partitions + 0.5);
  profile->GetXaxis()->SetTitle("i_{#eta}");

  for (Int_t iEta = 1; iEta <= num_eta_partitions; iEta++) {
    TString histName = residual_path + "_R" + iEta;
    TH1F* hist = getElement<TH1F>(getter, histName);
    Float_t mean = hist->GetMean();
    Float_t rms = hist->GetRMS();

    profile->SetBinContent(iEta, mean);
    profile->SetBinError(iEta, rms);
    profile->SetBinEntries(iEta, 1);

    profile->GetYaxis()->SetTitle(hist->GetXaxis()->GetTitle());
  }

  booker.setCurrentFolder(folder.Data());
  booker.bookProfile(name, profile);
}
