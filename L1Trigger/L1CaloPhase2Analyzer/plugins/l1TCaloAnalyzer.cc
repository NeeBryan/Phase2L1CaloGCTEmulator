#include "L1Trigger/L1CaloPhase2Analyzer/interface/L1TCaloAnalyzer.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

L1TCaloAnalyzer::L1TCaloAnalyzer(const edm::ParameterSet& cfg)
    : gct1slr3Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT1SLR3PostIP1"))),
      gct1slr1Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT1SLR1PostIP1"))),
      gct2slr3Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT2SLR3PostIP1"))),
      gct2slr1Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT2SLR1PostIP1"))),
      gct3slr3Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT3SLR3PostIP1"))),
      gct3slr1Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT3SLR1PostIP1"))),
      folderName_(cfg.getUntrackedParameter<std::string>("folderName", "GCTAnalyzer")) {
  usesResource("TFileService");
}

void L1TCaloAnalyzer::beginJob() {
  linkTree_ = tfs_->make<TTree>("linkTree", "GCT Post-IP1 Link Tree");

  linkTree_->Branch("run", &run_, "run/I");
  linkTree_->Branch("lumi", &lumi_, "lumi/I");
  linkTree_->Branch("event", &event_, "event/I");

  linkTree_->Branch("region_index", &region_index_);
  linkTree_->Branch("region_name", &region_name_);
  linkTree_->Branch("word_index", &word_index_);
  linkTree_->Branch("link_type", &link_type_);

  linkTree_->Branch("raw_link_word_low32", &raw_link_word_low32_);

  linkTree_->Branch("eg_energy", &eg_energy_);
  linkTree_->Branch("eg_eta", &eg_eta_);
  linkTree_->Branch("eg_phi", &eg_phi_);
  linkTree_->Branch("eg_hoe", &eg_hoe_);
  linkTree_->Branch("eg_hoeWP", &eg_hoeWP_);
  linkTree_->Branch("eg_iso", &eg_iso_);
  linkTree_->Branch("eg_isoWP", &eg_isoWP_);
  linkTree_->Branch("eg_fb", &eg_fb_);
  linkTree_->Branch("eg_timing", &eg_timing_);
  linkTree_->Branch("eg_shapeWP", &eg_shapeWP_);
  linkTree_->Branch("eg_brems", &eg_brems_);
  linkTree_->Branch("eg_spare", &eg_spare_);

  linkTree_->Branch("pf_energy", &pf_energy_);
  linkTree_->Branch("pf_eta", &pf_eta_);
  linkTree_->Branch("pf_phi", &pf_phi_);
  linkTree_->Branch("pf_hoe", &pf_hoe_);
  linkTree_->Branch("pf_ecal", &pf_ecal_);
  linkTree_->Branch("pf_hcal", &pf_hcal_);
  linkTree_->Branch("pf_spare", &pf_spare_);

  linkTree_->Branch("st_energy", &st_energy_);
  linkTree_->Branch("st_emEnergy", &st_emEnergy_);
  linkTree_->Branch("st_eta", &st_eta_);
  linkTree_->Branch("st_phi", &st_phi_);
  linkTree_->Branch("st_flags", &st_flags_);
}

void L1TCaloAnalyzer::fillRegion(const LinkCollection& coll, int regionIdx, const std::string& regionName) {
  edm::LogPrint("L1TCaloAnalyzer")
      << "Filling region " << regionName << " (index " << regionIdx << "), size = " << coll.size();

  for (unsigned int iWord = 0; iWord < coll.size(); ++iWord) {
    const LinkWord word = coll[iWord].data();

    region_index_.push_back(regionIdx);
    region_name_.push_back(regionName);
    word_index_.push_back(static_cast<int>(iWord));
    raw_link_word_low32_.push_back(static_cast<int>(word));

    std::vector<int> egEnergy, egEta, egPhi, egHoe, egHoeWP, egIso, egIsoWP, egFb, egTiming, egShapeWP, egBrems, egSpare;
    std::vector<int> pfEnergy, pfEta, pfPhi, pfHoe, pfEcal, pfHcal, pfSpare;
    std::vector<int> stEnergy, stEmEnergy, stEta, stPhi, stFlags;

    // Based on the firmware testbench / IP1 output convention:
    // EG links: 0..3
    // PF links: 3..8
    // ST links: 13..20
    if (iWord <= 3) {
      link_type_.push_back("EG");
      decodeEG(word, egEnergy, egEta, egPhi, egHoe, egHoeWP, egIso, egIsoWP,
               egFb, egTiming, egShapeWP, egBrems, egSpare);
    } else if (iWord >= 4 && iWord <= 8) {
      link_type_.push_back("PF");
      decodePF(word, pfEnergy, pfEta, pfPhi, pfHoe, pfEcal, pfHcal, pfSpare);
    } else if (iWord >= 13 && iWord <= 20) {
      link_type_.push_back("ST");
      decodeST(word, stEnergy, stEmEnergy, stEta, stPhi, stFlags);
    } else {
      link_type_.push_back("OTHER");
    }

    eg_energy_.push_back(egEnergy);
    eg_eta_.push_back(egEta);
    eg_phi_.push_back(egPhi);
    eg_hoe_.push_back(egHoe);
    eg_hoeWP_.push_back(egHoeWP);
    eg_iso_.push_back(egIso);
    eg_isoWP_.push_back(egIsoWP);
    eg_fb_.push_back(egFb);
    eg_timing_.push_back(egTiming);
    eg_shapeWP_.push_back(egShapeWP);
    eg_brems_.push_back(egBrems);
    eg_spare_.push_back(egSpare);

    pf_energy_.push_back(pfEnergy);
    pf_eta_.push_back(pfEta);
    pf_phi_.push_back(pfPhi);
    pf_hoe_.push_back(pfHoe);
    pf_ecal_.push_back(pfEcal);
    pf_hcal_.push_back(pfHcal);
    pf_spare_.push_back(pfSpare);

    st_energy_.push_back(stEnergy);
    st_emEnergy_.push_back(stEmEnergy);
    st_eta_.push_back(stEta);
    st_phi_.push_back(stPhi);
    st_flags_.push_back(stFlags);
  }
}

void L1TCaloAnalyzer::analyze(const edm::Event& evt, const edm::EventSetup&) {
  run_ = evt.id().run();
  lumi_ = evt.id().luminosityBlock();
  event_ = evt.id().event();

  region_index_.clear();
  region_name_.clear();
  word_index_.clear();
  link_type_.clear();
  raw_link_word_low32_.clear();

  eg_energy_.clear();
  eg_eta_.clear();
  eg_phi_.clear();
  eg_hoe_.clear();
  eg_hoeWP_.clear();
  eg_iso_.clear();
  eg_isoWP_.clear();
  eg_fb_.clear();
  eg_timing_.clear();
  eg_shapeWP_.clear();
  eg_brems_.clear();
  eg_spare_.clear();

  pf_energy_.clear();
  pf_eta_.clear();
  pf_phi_.clear();
  pf_hoe_.clear();
  pf_ecal_.clear();
  pf_hcal_.clear();
  pf_spare_.clear();

  st_energy_.clear();
  st_emEnergy_.clear();
  st_eta_.clear();
  st_phi_.clear();
  st_flags_.clear();

  edm::Handle<LinkCollection> hGCT1SLR3;
  edm::Handle<LinkCollection> hGCT1SLR1;
  edm::Handle<LinkCollection> hGCT2SLR3;
  edm::Handle<LinkCollection> hGCT2SLR1;
  edm::Handle<LinkCollection> hGCT3SLR3;
  edm::Handle<LinkCollection> hGCT3SLR1;

  const bool got1 = evt.getByToken(gct1slr3Src_, hGCT1SLR3);
  const bool got2 = evt.getByToken(gct1slr1Src_, hGCT1SLR1);
  const bool got3 = evt.getByToken(gct2slr3Src_, hGCT2SLR3);
  const bool got4 = evt.getByToken(gct2slr1Src_, hGCT2SLR1);
  const bool got5 = evt.getByToken(gct3slr3Src_, hGCT3SLR3);
  const bool got6 = evt.getByToken(gct3slr1Src_, hGCT3SLR1);

  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT1SLR3PostIP1 valid = " << got1 << (got1 ? ", size = " + std::to_string(hGCT1SLR3->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT1SLR1PostIP1 valid = " << got2 << (got2 ? ", size = " + std::to_string(hGCT1SLR1->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT2SLR3PostIP1 valid = " << got3 << (got3 ? ", size = " + std::to_string(hGCT2SLR3->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT2SLR1PostIP1 valid = " << got4 << (got4 ? ", size = " + std::to_string(hGCT2SLR1->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT3SLR3PostIP1 valid = " << got5 << (got5 ? ", size = " + std::to_string(hGCT3SLR3->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT3SLR1PostIP1 valid = " << got6 << (got6 ? ", size = " + std::to_string(hGCT3SLR1->size()) : "");

  if (got1) fillRegion(*hGCT1SLR3, 0, "GCT1SLR3PostIP1");
  if (got2) fillRegion(*hGCT1SLR1, 1, "GCT1SLR1PostIP1");
  if (got3) fillRegion(*hGCT2SLR3, 2, "GCT2SLR3PostIP1");
  if (got4) fillRegion(*hGCT2SLR1, 3, "GCT2SLR1PostIP1");
  if (got5) fillRegion(*hGCT3SLR3, 4, "GCT3SLR3PostIP1");
  if (got6) fillRegion(*hGCT3SLR1, 5, "GCT3SLR1PostIP1");

  linkTree_->Fill();
}

void L1TCaloAnalyzer::endJob() {}

void L1TCaloAnalyzer::decodeEG(const LinkWord& data,
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
                               std::vector<int>& spare) const {
  energy.clear();
  eta.clear();
  phi.clear();
  hoe.clear();
  hoeWP.clear();
  iso.clear();
  isoWP.clear();
  fb.clear();
  timing.clear();
  shapeWP.clear();
  brems.clear();
  spare.clear();

  for (int i = 0; i < 9; ++i) {
    const int start = i * 64;
    energy.push_back( static_cast<int>(data.range(start + 11, start +  0)));
    eta.push_back(    static_cast<int>(data.range(start + 18, start + 12)));
    phi.push_back(    static_cast<int>(data.range(start + 25, start + 19)));
    hoe.push_back(    static_cast<int>(data.range(start + 29, start + 26)));
    hoeWP.push_back(  static_cast<int>(data.range(start + 31, start + 30)));
    iso.push_back(    static_cast<int>(data.range(start + 34, start + 32)));
    isoWP.push_back(  static_cast<int>(data.range(start + 36, start + 35)));
    fb.push_back(     static_cast<int>(data.range(start + 42, start + 37)));
    timing.push_back( static_cast<int>(data.range(start + 47, start + 43)));
    shapeWP.push_back(static_cast<int>(data.range(start + 49, start + 48)));
    brems.push_back(  static_cast<int>(data.range(start + 51, start + 50)));
    spare.push_back(  static_cast<int>(data.range(start + 63, start + 52)));
  }
}

void L1TCaloAnalyzer::decodePF(const LinkWord& data,
                               std::vector<int>& energy,
                               std::vector<int>& eta,
                               std::vector<int>& phi,
                               std::vector<int>& hoe,
                               std::vector<int>& ecal,
                               std::vector<int>& hcal,
                               std::vector<int>& spare) const {
  energy.clear();
  eta.clear();
  phi.clear();
  hoe.clear();
  ecal.clear();
  hcal.clear();
  spare.clear();

  for (int i = 0; i < 9; ++i) {
    const int start = i * 64;
    energy.push_back(static_cast<int>(data.range(start + 11, start +  0)));
    eta.push_back(   static_cast<int>(data.range(start + 19, start + 12)));
    phi.push_back(   static_cast<int>(data.range(start + 26, start + 20)));
    hoe.push_back(   static_cast<int>(data.range(start + 30, start + 27)));
    ecal.push_back(  static_cast<int>(data.range(start + 42, start + 31)));
    hcal.push_back(  static_cast<int>(data.range(start + 54, start + 43)));
    spare.push_back( static_cast<int>(data.range(start + 63, start + 55)));
  }
}

void L1TCaloAnalyzer::decodeST(const LinkWord& data,
                               std::vector<int>& energy,
                               std::vector<int>& emEnergy,
                               std::vector<int>& eta,
                               std::vector<int>& phi,
                               std::vector<int>& flags) const {
  energy.clear();
  emEnergy.clear();
  eta.clear();
  phi.clear();
  flags.clear();

  for (int i = 0; i < 12; ++i) {
    const int start = i * 48;
    energy.push_back(  static_cast<int>(data.range(start + 11, start +  0)));
    emEnergy.push_back(static_cast<int>(data.range(start + 23, start + 12)));
    eta.push_back(     static_cast<int>(data.range(start + 27, start + 24)));
    phi.push_back(     static_cast<int>(data.range(start + 32, start + 28)));
    flags.push_back(   static_cast<int>(data.range(start + 47, start + 33)));
  }
}

DEFINE_FWK_MODULE(L1TCaloAnalyzer);