#!/bin/tcsh
cd /afs/cern.ch/work/b/bmarzocc/CMSSW_6_2_0/src/Validation/EcalValidation/test/crab/lxbatch
setenv SCRAM_ARCH slc5_amd64_gcc462
eval `scramv1 ru -csh`
cd -
cmsMkdir /store/caf/user/bmarzocc/MinBias_CMSSW_7_0_0_pre9_scEtThr/00000
cmsRun /afs/cern.ch/work/b/bmarzocc/CMSSW_6_2_0/src/Validation/EcalValidation/test/crab/lxbatch/00000/JOB_82/ecalvalidationDigilessMC_TEMPLATE_cfg.py
cmsStage EcalValidation_82.root /store/caf/user/bmarzocc/MinBias_CMSSW_7_0_0_pre9_scEtThr/00000
