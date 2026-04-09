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

#include "DataFormats/L1TCalorimeterPhase2/interface/GCT_output.h"

class L1TCaloAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit L1TCaloAnalyzer(const edm::ParameterSet& ps);
  ~L1TCaloAnalyzer() override = default;

  void analyze(const edm::Event& evt, const edm::EventSetup& es) override;
  void beginJob() override;
  void endJob() override;

private:
  using LinkCollection = l1tp2::gctOutputLinkCollection;
  using LinkWord = ap_uint<576>;

  void decodeEG(const LinkWord& data,
                std::vector<int>& energy,
                std::vector<int>& eta,
                std::vector<int>& phi,
                std::vector<int>& hoe,
                std::vector<int>& hoeWP,
                std::vector<int>& iso,
                std::vector<int>& isoWP,
                std::vector<int>& fb,
                std::vector<int>& timing,
                std::vector<int>& shapeWP,
                std::vector<int>& brems,
                std::vector<int>& spare) const;

  void decodePF(const LinkWord& data,
                std::vector<int>& energy,
                std::vector<int>& eta,
                std::vector<int>& phi,
                std::vector<int>& hoe,
                std::vector<int>& ecal,
                std::vector<int>& hcal,
                std::vector<int>& spare) const;

  void decodeST(const LinkWord& data,
                std::vector<int>& energy,
                std::vector<int>& emEnergy,
                std::vector<int>& eta,
                std::vector<int>& phi,
                std::vector<int>& flags) const;

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

  std::vector<int> region_index_;
  std::vector<std::string> region_name_;
  std::vector<int> word_index_;
  std::vector<std::string> link_type_;
  std::vector<int> raw_link_word_low32_;

  // EG decoded
  std::vector<std::vector<int>> eg_energy_;
  std::vector<std::vector<int>> eg_eta_;
  std::vector<std::vector<int>> eg_phi_;
  std::vector<std::vector<int>> eg_hoe_;
  std::vector<std::vector<int>> eg_hoeWP_;
  std::vector<std::vector<int>> eg_iso_;
  std::vector<std::vector<int>> eg_isoWP_;
  std::vector<std::vector<int>> eg_fb_;
  std::vector<std::vector<int>> eg_timing_;
  std::vector<std::vector<int>> eg_shapeWP_;
  std::vector<std::vector<int>> eg_brems_;
  std::vector<std::vector<int>> eg_spare_;

  // PF decoded
  std::vector<std::vector<int>> pf_energy_;
  std::vector<std::vector<int>> pf_eta_;
  std::vector<std::vector<int>> pf_phi_;
  std::vector<std::vector<int>> pf_hoe_;
  std::vector<std::vector<int>> pf_ecal_;
  std::vector<std::vector<int>> pf_hcal_;
  std::vector<std::vector<int>> pf_spare_;

  // ST decoded
  std::vector<std::vector<int>> st_energy_;
  std::vector<std::vector<int>> st_emEnergy_;
  std::vector<std::vector<int>> st_eta_;
  std::vector<std::vector<int>> st_phi_;
  std::vector<std::vector<int>> st_flags_;
};

#endif