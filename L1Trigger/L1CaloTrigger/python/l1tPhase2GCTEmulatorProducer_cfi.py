import FWCore.ParameterSet.Config as cms

l1tPhase2GCTEmulatorProducer = cms.EDProducer(
    "Phase2L1CaloL1GCTEmulator",
    LinkOut0=cms.InputTag("l1tPhase2RCTEmulatorProducer", "LinkOut0"),
    LinkOut1=cms.InputTag("l1tPhase2RCTEmulatorProducer", "LinkOut1"),
    LinkOut2=cms.InputTag("l1tPhase2RCTEmulatorProducer", "LinkOut2"),
    LinkOut3=cms.InputTag("l1tPhase2RCTEmulatorProducer", "LinkOut3"),
)