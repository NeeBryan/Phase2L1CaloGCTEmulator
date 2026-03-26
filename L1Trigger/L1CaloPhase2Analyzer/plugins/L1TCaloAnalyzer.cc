#include "L1Trigger/L1CaloPhase2Analyzer/interface/L1TCaloAnalyzer.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

L1TCaloAnalyzer::L1TCaloAnalyzer(const edm::ParameterSet& cfg)
    : gct1slr3Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT1SLR3PreIP1"))),
      gct1slr1Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT1SLR1PreIP1"))),
      gct2slr3Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT2SLR3PreIP1"))),
      gct2slr1Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT2SLR1PreIP1"))),
      gct3slr3Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT3SLR3PreIP1"))),
      gct3slr1Src_(consumes<LinkCollection>(cfg.getParameter<edm::InputTag>("GCT3SLR1PreIP1"))),
      folderName_(cfg.getUntrackedParameter<std::string>("folderName", "GCTAnalyzer")) {
  usesResource("TFileService");
}

void L1TCaloAnalyzer::beginJob() {
  linkTree_ = tfs_->make<TTree>("linkTree", "GCT Pre-IP1 Link Tree");

  linkTree_->Branch("run", &run_, "run/I");
  linkTree_->Branch("lumi", &lumi_, "lumi/I");
  linkTree_->Branch("event", &event_, "event/I");

  linkTree_->Branch("region_index", &region_index_);
  linkTree_->Branch("region_name", &region_name_);
  linkTree_->Branch("word_index", &word_index_);
  linkTree_->Branch("phi_slot", &phi_slot_);
  linkTree_->Branch("eta_side_slot", &eta_side_slot_);
  linkTree_->Branch("local_link_index", &local_link_index_);

  linkTree_->Branch("raw_link_word_low32", &raw_link_word_low32_);

  linkTree_->Branch("cluster_seed_pt", &cluster_seed_pt_);
  linkTree_->Branch("cluster_pt", &cluster_pt_);
  linkTree_->Branch("cluster_eta", &cluster_eta_);
  linkTree_->Branch("cluster_phi", &cluster_phi_);
  linkTree_->Branch("cluster_et5x5", &cluster_et5x5_);
  linkTree_->Branch("cluster_wps", &cluster_wps_);
  linkTree_->Branch("cluster_timing", &cluster_timing_);
  linkTree_->Branch("cluster_spike", &cluster_spike_);
  linkTree_->Branch("cluster_satur", &cluster_satur_);
  linkTree_->Branch("cluster_brems", &cluster_brems_);
  linkTree_->Branch("cluster_spare", &cluster_spare_);

  linkTree_->Branch("tower_et", &tower_et_);
  linkTree_->Branch("tower_eta", &tower_eta_);
  linkTree_->Branch("tower_phi", &tower_phi_);
  linkTree_->Branch("tower_hoe", &tower_hoe_);
  linkTree_->Branch("tower_fb", &tower_fb_);
}

void L1TCaloAnalyzer::fillRegion(const LinkCollection& coll, int regionIdx, const std::string& regionName) {
  edm::LogPrint("L1TCaloAnalyzer")
      << "Filling region " << regionName << " (index " << regionIdx << "), size = " << coll.size();

  for (unsigned int iWord = 0; iWord < coll.size(); ++iWord) {
    const LinkWord word = coll[iWord].data();

    // region bundle layout:
    //   phiSlot = iWord / 8
    //   within phi slot: 2 cards x 4 links
    //   etaSideSlot = (iWord % 8) / 4   -> 0 or 1
    //   localLink   = (iWord % 4)       -> 0,1,2,3
    const int phiSlot = static_cast<int>(iWord) / 8;
    const int etaSideSlot = (static_cast<int>(iWord) % 8) / 4;
    const int localLink = static_cast<int>(iWord) % 4;

    region_index_.push_back(regionIdx);
    region_name_.push_back(regionName);
    word_index_.push_back(static_cast<int>(iWord));
    phi_slot_.push_back(phiSlot);
    eta_side_slot_.push_back(etaSideSlot);
    local_link_index_.push_back(localLink);

    raw_link_word_low32_.push_back(static_cast<int>(word));

    std::vector<int> seedPt, pt, eta, phi, et5x5, wps, timing, spike, satur, brems, spare;
    std::vector<int> towerEt, towerEta, towerPhi, towerHoe, towerFb;

    if (localLink == 0) {
      decodeClusters(word, seedPt, pt, eta, phi, et5x5, wps, timing, spike, satur, brems, spare);
    } else {
      decodeTowers(word, localLink, towerEt, towerEta, towerPhi, towerHoe, towerFb);
    }

    cluster_seed_pt_.push_back(seedPt);
    cluster_pt_.push_back(pt);
    cluster_eta_.push_back(eta);
    cluster_phi_.push_back(phi);
    cluster_et5x5_.push_back(et5x5);
    cluster_wps_.push_back(wps);
    cluster_timing_.push_back(timing);
    cluster_spike_.push_back(spike);
    cluster_satur_.push_back(satur);
    cluster_brems_.push_back(brems);
    cluster_spare_.push_back(spare);

    tower_et_.push_back(towerEt);
    tower_eta_.push_back(towerEta);
    tower_phi_.push_back(towerPhi);
    tower_hoe_.push_back(towerHoe);
    tower_fb_.push_back(towerFb);
  }
}

void L1TCaloAnalyzer::analyze(const edm::Event& evt, const edm::EventSetup&) {
  run_ = evt.id().run();
  lumi_ = evt.id().luminosityBlock();
  event_ = evt.id().event();

  edm::LogPrint("L1TCaloAnalyzer")
      << "\n========== L1TCaloAnalyzer event start ==========\n"
      << "Run = " << run_ << ", Lumi = " << lumi_ << ", Event = " << event_;

  region_index_.clear();
  region_name_.clear();
  word_index_.clear();
  phi_slot_.clear();
  eta_side_slot_.clear();
  local_link_index_.clear();
  raw_link_word_low32_.clear();

  cluster_seed_pt_.clear();
  cluster_pt_.clear();
  cluster_eta_.clear();
  cluster_phi_.clear();
  cluster_et5x5_.clear();
  cluster_wps_.clear();
  cluster_timing_.clear();
  cluster_spike_.clear();
  cluster_satur_.clear();
  cluster_brems_.clear();
  cluster_spare_.clear();

  tower_et_.clear();
  tower_eta_.clear();
  tower_phi_.clear();
  tower_hoe_.clear();
  tower_fb_.clear();

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
      << "GCT1SLR3PreIP1 valid = " << got1 << (got1 ? ", size = " + std::to_string(hGCT1SLR3->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT1SLR1PreIP1 valid = " << got2 << (got2 ? ", size = " + std::to_string(hGCT1SLR1->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT2SLR3PreIP1 valid = " << got3 << (got3 ? ", size = " + std::to_string(hGCT2SLR3->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT2SLR1PreIP1 valid = " << got4 << (got4 ? ", size = " + std::to_string(hGCT2SLR1->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT3SLR3PreIP1 valid = " << got5 << (got5 ? ", size = " + std::to_string(hGCT3SLR3->size()) : "");
  edm::LogPrint("L1TCaloAnalyzer")
      << "GCT3SLR1PreIP1 valid = " << got6 << (got6 ? ", size = " + std::to_string(hGCT3SLR1->size()) : "");

  if (got1) fillRegion(*hGCT1SLR3, 0, "GCT1SLR3PreIP1");
  if (got2) fillRegion(*hGCT1SLR1, 1, "GCT1SLR1PreIP1");
  if (got3) fillRegion(*hGCT2SLR3, 2, "GCT2SLR3PreIP1");
  if (got4) fillRegion(*hGCT2SLR1, 3, "GCT2SLR1PreIP1");
  if (got5) fillRegion(*hGCT3SLR3, 4, "GCT3SLR3PreIP1");
  if (got6) fillRegion(*hGCT3SLR1, 5, "GCT3SLR1PreIP1");

  edm::LogPrint("L1TCaloAnalyzer")
      << "Final sizes before Fill(): "
      << " nWords = " << region_index_.size()
      << ", cluster_pt outer size = " << cluster_pt_.size()
      << ", tower_et outer size = " << tower_et_.size();

  linkTree_->Fill();

  edm::LogPrint("L1TCaloAnalyzer")
      << "========== L1TCaloAnalyzer event end ==========\n";
}

void L1TCaloAnalyzer::endJob() {}

void L1TCaloAnalyzer::decodeClusters(const LinkWord& data,
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
                                     std::vector<int>& spare) const {
  seedPt.clear();
  clusterPt.clear();
  clusterEta.clear();
  clusterPhi.clear();
  et5x5.clear();
  wps.clear();
  timing.clear();
  spike.clear();
  satur.clear();
  brems.clear();
  spare.clear();

  for (int i = 0; i < 9; ++i) {
    const int start = i * 64;

    seedPt.push_back(    static_cast<int>(data.range(start +  9, start +  0)));
    clusterPt.push_back( static_cast<int>(data.range(start + 21, start + 10)));
    clusterEta.push_back(static_cast<int>(data.range(start + 28, start + 22)));
    clusterPhi.push_back(static_cast<int>(data.range(start + 33, start + 29)));
    et5x5.push_back(     static_cast<int>(data.range(start + 43, start + 34)));
    wps.push_back(       static_cast<int>(data.range(start + 50, start + 44)));
    timing.push_back(    static_cast<int>(data.range(start + 55, start + 51)));
    spike.push_back(     static_cast<int>(data.range(start + 56, start + 56)));
    satur.push_back(     static_cast<int>(data.range(start + 57, start + 57)));
    brems.push_back(     static_cast<int>(data.range(start + 59, start + 58)));
    spare.push_back(     static_cast<int>(data.range(start + 63, start + 60)));
  }
}

void L1TCaloAnalyzer::decodeTowers(const LinkWord& data,
                                   int whichLocalTowerLink,
                                   std::vector<int>& towerEt,
                                   std::vector<int>& towerEta,
                                   std::vector<int>& towerPhi,
                                   std::vector<int>& towerHoe,
                                   std::vector<int>& towerFb) const {
  towerEt.clear();
  towerEta.clear();
  towerPhi.clear();
  towerHoe.clear();
  towerFb.clear();

  // local tower links 1,2,3 -> phi pairs 0/1, 2/3, 4/5
  const int phiBase = 2 * (whichLocalTowerLink - 1);

  for (int i = 0; i < 17; ++i) {
    {
      const int phi = phiBase;
      const int start = i * 16;

      towerEt.push_back( static_cast<int>(data.range(start +  9, start +  0)));
      towerEta.push_back(i);
      towerPhi.push_back(phi);
      towerHoe.push_back(static_cast<int>(data.range(start + 13, start + 10)));
      towerFb.push_back( static_cast<int>(data.range(start + 15, start + 14)));
    }

    {
      const int phi = phiBase + 1;
      const int start = i * 16 + 272;

      towerEt.push_back( static_cast<int>(data.range(start +  9, start +  0)));
      towerEta.push_back(i);
      towerPhi.push_back(phi);
      towerHoe.push_back(static_cast<int>(data.range(start + 13, start + 10)));
      towerFb.push_back( static_cast<int>(data.range(start + 15, start + 14)));
    }
  }
}

DEFINE_FWK_MODULE(L1TCaloAnalyzer);