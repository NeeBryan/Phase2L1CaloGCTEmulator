/*
 * Description:
 *   Phase-2 GCT emulator, version 2:
 *   - consumes RCT output links (LinkOut0..3)
 *   - reorganizes them into pre-IP1 GCT input bundles
 *   - does NOT call GCT IP1 yet
 */

#include <ap_int.h>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/L1TCalorimeterPhase2/interface/RCT_output.h"

class Phase2L1CaloL1GCTEmulator : public edm::stream::EDProducer<> {
public:
  explicit Phase2L1CaloL1GCTEmulator(const edm::ParameterSet&);
  ~Phase2L1CaloL1GCTEmulator() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  using LinkCollection = l1tp2::rctOutputLinkCollection;
  using LinkWord = ap_uint<576>;

  edm::EDGetTokenT<LinkCollection> link0Src_;
  edm::EDGetTokenT<LinkCollection> link1Src_;
  edm::EDGetTokenT<LinkCollection> link2Src_;
  edm::EDGetTokenT<LinkCollection> link3Src_;

  static constexpr int kNRCTCards = 24;
  static constexpr int kNRCTCardPairs = 12;
  static constexpr int kNGCTRegions = 6;
  static constexpr int kLinksPerRCTCard = 4;
  static constexpr int kRCTCardsPerGCTRegion = 8;     // 4 phi x 2 eta
  static constexpr int kLinksPerGCTRegion = 32;       // 8 cards x 4 links

  LinkWord getLinkWord(
      const LinkCollection& link0,
      const LinkCollection& link1,
      const LinkCollection& link2,
      const LinkCollection& link3,
      int cardIdx,
      int whichLink) const;

  void appendCardLinks(
      std::array<LinkWord, kLinksPerGCTRegion>& out,
      int& writeIdx,
      const LinkCollection& link0,
      const LinkCollection& link1,
      const LinkCollection& link2,
      const LinkCollection& link3,
      int cardIdx) const;

  std::array<LinkWord, kLinksPerGCTRegion> buildRegionInput(
      const LinkCollection& link0,
      const LinkCollection& link1,
      const LinkCollection& link2,
      const LinkCollection& link3,
      int region) const;

  static int pairToNegCard(int pairIdx) { return 2 * pairIdx; }
  static int pairToPosCard(int pairIdx) { return 2 * pairIdx + 1; }

  static std::string outputLabel(int region);
};

Phase2L1CaloL1GCTEmulator::Phase2L1CaloL1GCTEmulator(const edm::ParameterSet& iConfig)
    : link0Src_(consumes<LinkCollection>(iConfig.getParameter<edm::InputTag>("LinkOut0"))),
      link1Src_(consumes<LinkCollection>(iConfig.getParameter<edm::InputTag>("LinkOut1"))),
      link2Src_(consumes<LinkCollection>(iConfig.getParameter<edm::InputTag>("LinkOut2"))),
      link3Src_(consumes<LinkCollection>(iConfig.getParameter<edm::InputTag>("LinkOut3"))) {
  produces<LinkCollection>("GCT1SLR3PreIP1");
  produces<LinkCollection>("GCT1SLR1PreIP1");
  produces<LinkCollection>("GCT2SLR3PreIP1");
  produces<LinkCollection>("GCT2SLR1PreIP1");
  produces<LinkCollection>("GCT3SLR3PreIP1");
  produces<LinkCollection>("GCT3SLR1PreIP1");
}

ap_uint<576> Phase2L1CaloL1GCTEmulator::getLinkWord(
    const LinkCollection& link0,
    const LinkCollection& link1,
    const LinkCollection& link2,
    const LinkCollection& link3,
    int cardIdx,
    int whichLink) const {
  if (cardIdx < 0 || cardIdx >= kNRCTCards) {
    throw cms::Exception("Phase2L1CaloL1GCTEmulator")
        << "Bad RCT card index: " << cardIdx;
  }

  if ((int)link0.size() <= cardIdx || (int)link1.size() <= cardIdx ||
      (int)link2.size() <= cardIdx || (int)link3.size() <= cardIdx) {
    throw cms::Exception("Phase2L1CaloL1GCTEmulator")
        << "RCT link collections do not have enough entries. "
        << "Need at least " << (cardIdx + 1) << " entries in each collection.";
  }

  switch (whichLink) {
    case 0: return link0[cardIdx].data();
    case 1: return link1[cardIdx].data();
    case 2: return link2[cardIdx].data();
    case 3: return link3[cardIdx].data();
    default:
      throw cms::Exception("Phase2L1CaloL1GCTEmulator")
          << "Bad link number: " << whichLink;
  }
}

void Phase2L1CaloL1GCTEmulator::appendCardLinks(
    std::array<LinkWord, kLinksPerGCTRegion>& out,
    int& writeIdx,
    const LinkCollection& link0,
    const LinkCollection& link1,
    const LinkCollection& link2,
    const LinkCollection& link3,
    int cardIdx) const {
  // Assumed per-card order before GCT IP1:
  //   link0 = EG clusters
  //   link1 = tower fiber 0
  //   link2 = tower fiber 1
  //   link3 = tower fiber 2
  out[writeIdx++] = getLinkWord(link0, link1, link2, link3, cardIdx, 0);
  out[writeIdx++] = getLinkWord(link0, link1, link2, link3, cardIdx, 1);
  out[writeIdx++] = getLinkWord(link0, link1, link2, link3, cardIdx, 2);
  out[writeIdx++] = getLinkWord(link0, link1, link2, link3, cardIdx, 3);
}

std::array<ap_uint<576>, Phase2L1CaloL1GCTEmulator::kLinksPerGCTRegion>
Phase2L1CaloL1GCTEmulator::buildRegionInput(
    const LinkCollection& link0,
    const LinkCollection& link1,
    const LinkCollection& link2,
    const LinkCollection& link3,
    int region) const {
  if (region < 0 || region >= kNGCTRegions) {
    throw cms::Exception("Phase2L1CaloL1GCTEmulator")
        << "Bad GCT region: " << region;
  }

  std::array<LinkWord, kLinksPerGCTRegion> out{};
  int writeIdx = 0;

  static constexpr int kRegionPairMap[6][4] = {
  {0, 1, 2, 3},    // SLR 1.3
  {4, 5, 6, 7},    // SLR 2.3
  {8, 9, 10, 11},  // SLR 3.3
  {10, 11, 0, 1},  // SLR 3.1
  {2, 3, 4, 5},    // SLR 1.1
  {6, 7, 8, 9}     // SLR 2.1
  };

  for (int phiSlot = 0; phiSlot < 4; ++phiSlot) {
  const int pairIdx = kRegionPairMap[region][phiSlot];
  const int negCard = pairToNegCard(pairIdx);
  const int posCard = pairToPosCard(pairIdx);

  appendCardLinks(out, writeIdx, link0, link1, link2, link3, posCard);
  appendCardLinks(out, writeIdx, link0, link1, link2, link3, negCard);
  }

  if (writeIdx != kLinksPerGCTRegion) {
    throw cms::Exception("Phase2L1CaloL1GCTEmulator")
        << "Internal error: wrote " << writeIdx
        << " links, expected " << kLinksPerGCTRegion;
  }

  return out;
}

std::string Phase2L1CaloL1GCTEmulator::outputLabel(int region) {
  switch (region) {
    case 0: return "GCT1SLR3PreIP1";
    case 1: return "GCT1SLR1PreIP1";
    case 2: return "GCT2SLR3PreIP1";
    case 3: return "GCT2SLR1PreIP1";
    case 4: return "GCT3SLR3PreIP1";
    case 5: return "GCT3SLR1PreIP1";
    default: return "INVALID";
  }
}

void Phase2L1CaloL1GCTEmulator::produce(edm::Event& iEvent, const edm::EventSetup&) {
  edm::Handle<LinkCollection> hLink0;
  edm::Handle<LinkCollection> hLink1;
  edm::Handle<LinkCollection> hLink2;
  edm::Handle<LinkCollection> hLink3;

  iEvent.getByToken(link0Src_, hLink0);
  iEvent.getByToken(link1Src_, hLink1);
  iEvent.getByToken(link2Src_, hLink2);
  iEvent.getByToken(link3Src_, hLink3);

  if (!hLink0.isValid() || !hLink1.isValid() || !hLink2.isValid() || !hLink3.isValid()) {
    throw cms::Exception("Phase2L1CaloL1GCTEmulator")
        << "Failed to get one or more RCT link collections.";
  }

  auto out0 = std::make_unique<LinkCollection>();
  auto out1 = std::make_unique<LinkCollection>();
  auto out2 = std::make_unique<LinkCollection>();
  auto out3 = std::make_unique<LinkCollection>();
  auto out4 = std::make_unique<LinkCollection>();
  auto out5 = std::make_unique<LinkCollection>();

  std::array<LinkCollection*, 6> outputs = {{
      out0.get(), out1.get(), out2.get(), out3.get(), out4.get(), out5.get()
  }};

  for (int region = 0; region < kNGCTRegions; ++region) {
    const auto regionLinks = buildRegionInput(*hLink0, *hLink1, *hLink2, *hLink3, region);

    for (const auto& word : regionLinks) {
      outputs[region]->emplace_back(word);
    }
  }

    edm::LogPrint("GCTEmulator") << "Input RCT sizes:"
                               << " LinkOut0=" << hLink0->size()
                               << " LinkOut1=" << hLink1->size()
                               << " LinkOut2=" << hLink2->size()
                               << " LinkOut3=" << hLink3->size();

  edm::LogPrint("GCTEmulator") << "About to put GCT1SLR3PreIP1 size = " << out0->size();
  edm::LogPrint("GCTEmulator") << "About to put GCT1SLR1PreIP1 size = " << out1->size();
  edm::LogPrint("GCTEmulator") << "About to put GCT2SLR3PreIP1 size = " << out2->size();
  edm::LogPrint("GCTEmulator") << "About to put GCT2SLR1PreIP1 size = " << out3->size();
  edm::LogPrint("GCTEmulator") << "About to put GCT3SLR3PreIP1 size = " << out4->size();
  edm::LogPrint("GCTEmulator") << "About to put GCT3SLR1PreIP1 size = " << out5->size();


  iEvent.put(std::move(out0), "GCT1SLR3PreIP1");
  iEvent.put(std::move(out1), "GCT1SLR1PreIP1");
  iEvent.put(std::move(out2), "GCT2SLR3PreIP1");
  iEvent.put(std::move(out3), "GCT2SLR1PreIP1");
  iEvent.put(std::move(out4), "GCT3SLR3PreIP1");
  iEvent.put(std::move(out5), "GCT3SLR1PreIP1");
}

void Phase2L1CaloL1GCTEmulator::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("LinkOut0", edm::InputTag("l1tPhase2RCTEmulatorProducer", "LinkOut0"));
  desc.add<edm::InputTag>("LinkOut1", edm::InputTag("l1tPhase2RCTEmulatorProducer", "LinkOut1"));
  desc.add<edm::InputTag>("LinkOut2", edm::InputTag("l1tPhase2RCTEmulatorProducer", "LinkOut2"));
  desc.add<edm::InputTag>("LinkOut3", edm::InputTag("l1tPhase2RCTEmulatorProducer", "LinkOut3"));
  descriptions.add("Phase2L1CaloL1GCTEmulator", desc);
}

DEFINE_FWK_MODULE(Phase2L1CaloL1GCTEmulator);