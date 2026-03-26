import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

from Configuration.StandardSequences.Eras import eras

options = VarParsing('analysis')
options.parseArguments()

process = cms.Process("L1AlgoTest", eras.Phase2C17I13M9)

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtendedRun4D110Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedRun4D110_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

if len(options.inputFiles) > 0:
    file_list = options.inputFiles
else:
    file_list = [
        "root://cmsxrootd.fnal.gov//store/mc/Phase2Spring24DIGIRECOMiniAOD/DoubleElectron_FlatPt-1To100-gun/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2810000/001ebf5f-b83c-43fc-997f-c2e5ecf1f9dd.root"
    ]

process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(*file_list),
    inputCommands=cms.untracked.vstring(
        "keep *",
        "drop l1tTkPrimaryVertexs_*_*_*",
    )
)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '140X_mcRun4_realistic_v4', '')

# Add HCAL Transcoder
process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')
process.load('CalibCalorimetry.CaloTPG.CaloTPGTranscoder_cfi')

# Run RCT emulator
process.load("L1Trigger.L1CaloTrigger.l1tPhase2RCTEmulatorProducer_cfi")

# Run GCT emulator
process.load("L1Trigger.L1CaloTrigger.l1tPhase2GCTEmulatorProducer_cfi")

# Main path
process.p = cms.Path(
    process.l1tPhase2RCTEmulatorProducer *
    process.l1tPhase2GCTEmulatorProducer
)


process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.options.numberOfThreads = cms.untracked.uint32(8)
process.options.numberOfStreams = cms.untracked.uint32(0)
process.options.wantSummary = cms.untracked.bool(True)