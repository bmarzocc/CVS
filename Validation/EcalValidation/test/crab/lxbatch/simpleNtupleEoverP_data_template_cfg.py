import FWCore.ParameterSet.Config as cms

from Calibration.EcalCalibNtuple.simpleNtupleEoverP_cff import *
from Calibration.EcalCalibNtuple.recoTags_cff import *

process = cms.Process("SimpleNtupleEoverP")

# flags
GlobalTag = "GR_R_62_V1::All"
runOverSandbox   = False
runOverAlcaReco  = False
runOverData      = True
saveRecHitMatrix = False

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
process.MessageLogger.cerr.threshold = cms.untracked.string("DEBUG")
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))

# simpleNtuple
makeSimpleNtuple(process,GlobalTag=GlobalTag,runOverSandbox=runOverSandbox,runOverAlcaReco=runOverAlcaReco,runOverData=runOverData)

#makeRecoTags(process)
#makeSqliteTags(process)

#HLT selection
import HLTrigger.HLTfilters.hltHighLevel_cfi as hlt
process.filter_1 = hlt.hltHighLevel.clone(
    HLTPaths = [ 'HLT_DoubleEle*'],
    throw = False
    )

# path
if not saveRecHitMatrix:
    process.simpleNtuple_step = cms.Path(
        #process.filter_1*
        process.simpleNtupleEoverP
        )
    
if saveRecHitMatrix:
    process.simpleNtuple_step = cms.Path(
        process.ecalDigis *
        process.ecalPreshowerDigis *
        process.hcalDigis *
        process.calolocalreco *
        process.simpleNtupleEoverP
        )
    
    process.simpleNtupleEoverP.saveFbrem        = cms.untracked.bool(True)
    process.simpleNtupleEoverP.saveRecHitMatrix = cms.untracked.bool(True)
    process.simpleNtupleEoverP.recHitCollection_EB = cms.InputTag("ecalRecHit","EcalRecHitsEB")
    process.simpleNtupleEoverP.recHitCollection_EE = cms.InputTag("ecalRecHit","EcalRecHitsEE")

# source
process.source.fileNames = cms.untracked.vstring(
    LISTOFFILES
    )

process.maxEvents = cms.untracked.PSet(
   input = cms.untracked.int32(-1)
)

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("OUTPUTFILENAME.root")
    )


process.schedule = cms.Schedule(
    process.simpleNtuple_step
    )
