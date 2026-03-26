import FWCore.ParameterSet.Config as cms

l1NtupleProducer = cms.EDAnalyzer(
    "L1TCaloAnalyzer",
    folderName = cms.untracked.string("GCTAnalyzer"),

    GCT1SLR3PreIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT1SLR3PreIP1"),
    GCT1SLR1PreIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT1SLR1PreIP1"),
    GCT2SLR3PreIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT2SLR3PreIP1"),
    GCT2SLR1PreIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT2SLR1PreIP1"),
    GCT3SLR3PreIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT3SLR3PreIP1"),
    GCT3SLR1PreIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT3SLR1PreIP1"),
)