import FWCore.ParameterSet.Config as cms
from Configuration.AlCa.GlobalTag import GlobalTag

process = cms.Process("Validation")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
process.MessageLogger.cerr = cms.untracked.PSet(threshold = cms.untracked.string("ERROR"))

# Geometry
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")
process.load("Geometry.CaloEventSetup.CaloGeometry_cff")

# initialize magnetic field
process.load("Configuration.StandardSequences.MagneticField_cff")

##noise    
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')  
                                                     
process.GlobalTag.globaltag = "START53_V7A::All"
                                                                       
process.GlobalTag.toGet = cms.VPSet(
            cms.PSet(record = cms.string("EcalPedestalsRcd"),
            tag = cms.string("EcalPedestals_206766_200_mc"),
            connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_ECAL")
            ),
            cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
            tag = cms.string("EcalLaserAPDPNRatios_20130130_447_p1_v2_run206859_mc"),
            connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_ECAL")
            ),
            cms.PSet(record = cms.string('EcalLaserAlphasRcd'),
            tag = cms.string('EcalLaserAlphas_EB_sic1_btcp152_EE_sic1_btcp116'),
            connect = cms.untracked.string('frontier://FrontierPrep/CMS_COND_ECAL')
            )
)

process.source = cms.Source("PoolSource",
    skipEvents = cms.untracked.uint32(0),                       
    fileNames = cms.untracked.vstring(
    "root://eoscms.cern.ch//eos/cms/store/relval/CMSSW_5_3_9_patch3/RelValZEE/GEN-SIM-DIGI-RAW-HLTDEBUG/START53_V7L_PhRD_24Apr2013-v1/00000/181A5DF8-95AD-E211-AF4A-003048FFD736.root"
    )
)



process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)


# filter on PhysDeclared bit
process.skimming = cms.EDFilter("PhysDecl",
   applyfilter = cms.untracked.bool(True)
)

# filter on bit 40 || 41 nad !(bit36 || bit37 || bit38 || bit39)
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')
process.hltLevel1GTSeed.L1TechTriggerSeeding = cms.bool(True)
process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('(40 OR 41) AND NOT (36 OR 37 OR 38 OR 39)')


#Good Vertex Filter (see GOODCOLL skim)
process.primaryVertexFilter = cms.EDFilter("VertexSelector",
  src = cms.InputTag("offlinePrimaryVertices"),
  cut = cms.string("!isFake && ndof > 3 && abs(z) <= 24 && position.Rho <= 2"), 
  filter = cms.bool(True)   
)

# FilterOutScraping
process.noscraping = cms.EDFilter("FilterOutScraping",
   applyfilter = cms.untracked.bool(True),
   debugOn = cms.untracked.bool(False),
   numtrack = cms.untracked.uint32(10),
   thresh = cms.untracked.double(0.25)
)

process.load("Validation.EcalValidation.ecalvalidation_cfi")

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('EcalValidation_RelValZEE_CMSSW_5_3_9_patch3-START53_V7L_PhRD_24Apr2013-v1_GEN-SIM-DIGI-RAW-HLTDEBUG.root')
)

process.p = cms.Path(
    #process.skimming*
    #process.hltLevel1GTSeed*
    #process.noscraping*
    #process.primaryVertexFilter*
    process.ecalvalidation
    )

