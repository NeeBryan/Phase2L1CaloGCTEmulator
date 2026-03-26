#ifndef L1Trigger_L1CaloPhase2Analyzer_L1TCaloAnalyzer_h
#define L1Trigger_L1CaloPhase2Analyzer_L1TCaloAnalyzer_h

#include <vector>
#include <string>

#include "TTree.h"
#include <ap_int.h>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/L1TCalorimeterPhase2/interface/RCT_output.h"

class L1TCaloAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit L1TCaloAnalyzer(const edm::ParameterSet& ps);
  ~L1TCaloAnalyzer() override = default;

  void analyze(const edm::Event& evt, const edm::EventSetup& es) override;
  void beginJob() override;
  void endJob() override;

private:
  using LinkCollection = l1tp2::rctOutputLinkCollection;
  using LinkWord = ap_uint<576>;

  void decodeClusters(const LinkWord& data,
                      std::vector<int>& seedPt,
                      std::vector<int>& clusterPt,
                      std::vector<int>& clusterEta,
                      std::vector<int>& clusterPhi,
                      std::vector<int>& et5x5,
                      std::vector<int>& wps,
                      std::vector<int>& timing,
                      std::vector<int>& spike,
                      std::vector<int>& satur,
                      std::vector<int>& brems,
                      std::vector<int>& spare) const;

  void decodeTowers(const LinkWord& data,
                    int whichLocalTowerLink,
                    std::vector<int>& towerEt,
                    std::vector<int>& towerEta,
                    std::vector<int>& towerPhi,
                    std::vector<int>& towerHoe,
                    std::vector<int>& towerFb) const;

  void fillRegion(const LinkCollection& coll, int regionIdx, const std::string& regionName);

  edm::EDGetTokenT<LinkCollection> gct1slr3Src_;
  edm::EDGetTokenT<LinkCollection> gct1slr1Src_;
  edm::EDGetTokenT<LinkCollection> gct2slr3Src_;
  edm::EDGetTokenT<LinkCollection> gct2slr1Src_;
  edm::EDGetTokenT<LinkCollection> gct3slr3Src_;
  edm::EDGetTokenT<LinkCollection> gct3slr1Src_;

  std::string folderName_;
  edm::Service<TFileService> tfs_;
  TTree* linkTree_{nullptr};

  int run_{0};
  int lumi_{0};
  int event_{0};

  // one entry per raw link word across all 6 GCT regions
  std::vector<int> region_index_;
  std::vector<std::string> region_name_;
  std::vector<int> word_index_;
  std::vector<int> phi_slot_;
  std::vector<int> eta_side_slot_;
  std::vector<int> local_link_index_;

  // raw low 32 bits, only for quick debugging
  std::vector<int> raw_link_word_low32_;

  // decoded clusters: outer vector = one entry per raw link word
  std::vector<std::vector<int>> cluster_seed_pt_;
  std::vector<std::vector<int>> cluster_pt_;
  std::vector<std::vector<int>> cluster_eta_;
  std::vector<std::vector<int>> cluster_phi_;
  std::vector<std::vector<int>> cluster_et5x5_;
  std::vector<std::vector<int>> cluster_wps_;
  std::vector<std::vector<int>> cluster_timing_;
  std::vector<std::vector<int>> cluster_spike_;
  std::vector<std::vector<int>> cluster_satur_;
  std::vector<std::vector<int>> cluster_brems_;
  std::vector<std::vector<int>> cluster_spare_;

  // decoded towers: outer vector = one entry per raw link word
  std::vector<std::vector<int>> tower_et_;
  std::vector<std::vector<int>> tower_eta_;
  std::vector<std::vector<int>> tower_phi_;
  std::vector<std::vector<int>> tower_hoe_;
  std::vector<std::vector<int>> tower_fb_;
};

#endif