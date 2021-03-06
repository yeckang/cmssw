#ifndef DQMServices_ClientConfig_DQMGenericClient_H
#define DQMServices_ClientConfig_DQMGenericClient_H

/*
 *  Class:DQMGenericClient 
 *
 *  DQM histogram post processor
 *
 *
 *  \author Junghwan Goh - SungKyunKwan University
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include <set>
#include <string>
#include <vector>
#include <TH1.h>
#include <RVersion.h>
#if ROOT_VERSION_CODE >= ROOT_VERSION(5, 27, 0)
#include <TEfficiency.h>
#else
#include <TGraphAsymmErrors.h>
#endif

class MonitorElement;

class DQMGenericClient : public DQMEDHarvester {
public:
  DQMGenericClient(const edm::ParameterSet& pset);
  ~DQMGenericClient() override{};

  void dqmEndLuminosityBlock(DQMStore::IBooker& ibooker,
                             DQMStore::IGetter& igetter,
                             const edm::LuminosityBlock& lumiSeg,
                             const edm::EventSetup& c) override;
  void dqmEndJob(DQMStore::IBooker&, DQMStore::IGetter&) override;

  enum class EfficType { none = 0, efficiency, fakerate, simpleratio };

  struct EfficOption {
    std::string name, title;
    std::string numerator, denominator;
    EfficType type;
    bool isProfile;
  };

  struct ResolOption {
    std::string namePrefix, titlePrefix;
    std::string srcName;
  };

  struct ProfileOption {
    std::string name, title;
    std::string srcName;
  };

  struct NormOption {
    std::string name, normHistName;
  };

  struct CDOption {
    std::string name;
    bool ascending;
  };

  struct NoFlowOption {
    std::string name;
  };

  void computeEfficiency(DQMStore::IBooker& ibooker,
                         DQMStore::IGetter& igetter,
                         const std::string& startDir,
                         const std::string& efficMEName,
                         const std::string& efficMETitle,
                         const std::string& recoMEName,
                         const std::string& simMEName,
                         const EfficType type = EfficType::efficiency,
                         const bool makeProfile = false);
  void computeResolution(DQMStore::IBooker& ibooker,
                         DQMStore::IGetter& igetter,
                         const std::string& startDir,
                         const std::string& fitMEPrefix,
                         const std::string& fitMETitlePrefix,
                         const std::string& srcMEName);
  void computeProfile(DQMStore::IBooker& ibooker,
                      DQMStore::IGetter& igetter,
                      const std::string& startDir,
                      const std::string& profileMEName,
                      const std::string& profileMETitle,
                      const std::string& srcMEName);

  void normalizeToEntries(DQMStore::IBooker& ibooker,
                          DQMStore::IGetter& igetter,
                          const std::string& startDir,
                          const std::string& histName,
                          const std::string& normHistName);
  void makeCumulativeDist(DQMStore::IBooker& ibooker,
                          DQMStore::IGetter& igetter,
                          const std::string& startDir,
                          const std::string& cdName,
                          bool ascending = true);
  void makeNoFlowDist(DQMStore::IBooker& ibooker,
                      DQMStore::IGetter& igetter,
                      const std::string& startDir,
                      const std::string& cdName);

  void limitedFit(MonitorElement* srcME, MonitorElement* meanME, MonitorElement* sigmaME);

private:
  unsigned int verbose_;
  bool runOnEndLumi_;
  bool runOnEndJob_;
  bool makeGlobalEffPlot_;
  bool isWildcardUsed_;
  bool resLimitedFit_;

  DQMStore* theDQM;
  std::vector<std::string> subDirs_;
  std::string outputFileName_;

  std::vector<EfficOption> efficOptions_;
  std::vector<ResolOption> resolOptions_;
  std::vector<ProfileOption> profileOptions_;
  std::vector<NormOption> normOptions_;
  std::vector<CDOption> cdOptions_;
  std::vector<NoFlowOption> noFlowOptions_;

  void generic_eff(TH1* denom, TH1* numer, MonitorElement* efficiencyHist, const EfficType type = EfficType::efficiency);

  void findAllSubdirectories(DQMStore::IBooker& ibooker,
                             DQMStore::IGetter& igetter,
                             std::string dir,
                             std::set<std::string>* myList,
                             const TString& pattern);

  void makeAllPlots(DQMStore::IBooker&, DQMStore::IGetter&);

  void removeMEIfBooked(const std::string& meName, DQMStore::IGetter& igetter);

#if ROOT_VERSION_CODE >= ROOT_VERSION(5, 27, 0)

#else
  class TGraphAsymmErrorsWrapper : public TGraphAsymmErrors {
  public:
    std::pair<double, double> efficiency(int numerator, int denominator) {
      double eff, low, high;
      Efficiency(numerator, denominator, 0.683, eff, low, high);
      double error = (eff - low > high - eff) ? eff - low : high - eff;
      return std::pair<double, double>(eff, error);
    }
  };
#endif
};

#endif

/* vim:set ts=2 sts=2 sw=2 expandtab: */
