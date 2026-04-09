import FWCore.ParameterSet.Config as cms

l1NtupleProducer = cms.EDAnalyzer(
    "L1TCaloAnalyzer",
    folderName = cms.untracked.string("GCTAnalyzer"),

    GCT1SLR3PostIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT1SLR3PostIP1"),
    GCT1SLR1PostIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT1SLR1PostIP1"),
    GCT2SLR3PostIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT2SLR3PostIP1"),
    GCT2SLR1PostIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT2SLR1PostIP1"),
    GCT3SLR3PostIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT3SLR3PostIP1"),
    GCT3SLR1PostIP1 = cms.InputTag("l1tPhase2GCTEmulatorProducer", "GCT3SLR1PostIP1"),
)