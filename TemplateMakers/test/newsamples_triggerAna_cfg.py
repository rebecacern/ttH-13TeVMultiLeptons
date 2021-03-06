import FWCore.ParameterSet.Config as cms
import sys

process = cms.Process("Demo")

#old samples
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'PLS170_V7AN1::All'  ###'PLS170_V7AN1::All' ##'START61_V11::All' #START61_V8::All #'GR_R_60_V7::All'   # 'GR_R_52_V9::All'
process.prefer("GlobalTag")

#new samples
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag as customiseGlobalTag 
#process.GlobalTag = customiseGlobalTag(process.GlobalTag, globaltag = 'MCRUN2_72_V1A::All') ## maybe try MCRUN2_72_V1? "optimisitic" conditions.
#process.GlobalTag.connect = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'


process.maxEvents = cms.untracked.PSet(
    	input = cms.untracked.int32(100) # number of events
)

miniaodfiledir = str("/store/user/gesmith/samples/mc/Spring14miniaod/TTbarH_M-125_13TeV_amcatnlo-pythia8-tauola/MINIAODSIM/PU40bx25_PHYS14_25_V1-v1/00000/")
#aodfiledir = str("file:/afs/cern.ch/user/g/gesmith/earth/hadoop/users/gsmith15/mc/Phys14DR/TTbarH_M-125_13TeV_amcatnlo-pythia8-tauola/AODSIM/PU40bx25_PHYS14_25_V1-v1/00000/")
aodfiledir = str("root://xrootd.ba.infn.it//store/mc/Phys14DR/TTbarH_M-125_13TeV_amcatnlo-pythia8-tauola/AODSIM/PU40bx25_PHYS14_25_V1-v1/00000/")

process.source = cms.Source("PoolSource",
    	fileNames = cms.untracked.vstring(
	#aod:
	aodfiledir + '0456F76A-EA77-E411-81F8-0025B31E3D3C.root',
	aodfiledir + '10C19613-E176-E411-92F7-F04DA23BBCCA.root',
	aodfiledir + '18F3D0C9-0177-E411-81D0-002590A37114.root',
	aodfiledir + '1CA79CF9-F876-E411-BD55-001E67396E05.root',
	aodfiledir + '28D3938A-F776-E411-B51A-00259020083C.root',
	aodfiledir + '28D5EAFC-F976-E411-A45C-002590A887EE.root',
	aodfiledir + '32AE7681-FC76-E411-A940-002481E14D64.root',
	aodfiledir + '36B52DAC-0077-E411-AA51-001E67396793.root',
	aodfiledir + '3CD5D12A-0877-E411-8C96-001E67397D64.root',
	aodfiledir + '4057BA6C-F876-E411-B260-0025902008C8.root',
	aodfiledir + '4059ABE0-E676-E411-81E5-002590A4C6BE.root',
	aodfiledir + '422588BA-F376-E411-82B1-001E67397AE4.root',
	aodfiledir + '46324C48-F076-E411-8143-001E67396892.root',
	aodfiledir + '507A8AF3-D276-E411-87E5-002590A370B2.root',
	aodfiledir + '68F5DFAD-FD76-E411-AAE0-002590200AE0.root',
	aodfiledir + '6C4F596C-E576-E411-B749-002590A4C69A.root',
	aodfiledir + '6E125622-FF76-E411-A4CC-001E67397747.root',
	aodfiledir + '70A88054-0F77-E411-A2DC-002590A8312A.root',
	aodfiledir + '70D0E1C9-DE76-E411-B619-00259020083C.root',
	aodfiledir + '76324C40-D776-E411-8F75-001E67397215.root',
	aodfiledir + '766E4FF0-FB76-E411-8E3D-002590200A58.root',
	aodfiledir + '7C93250E-F676-E411-8D70-002590A3711C.root',
	aodfiledir + '981D0C32-1778-E411-85B6-001E67397431.root',
	aodfiledir + '9A8376E4-E876-E411-B51D-001E67396905.root',
	aodfiledir + '9E1F21FE-ED76-E411-AC2A-001E67396973.root',
	aodfiledir + 'A453C147-F776-E411-ABB1-001E67397A76.root',
	aodfiledir + 'B020C664-E376-E411-920A-001E67396581.root',
	aodfiledir + 'BE2C75D8-EC76-E411-9507-002590A371AE.root',
	aodfiledir + 'C0D697DA-EE76-E411-A7E0-002590200ABC.root',
	aodfiledir + 'C22944BE-FA76-E411-81DF-002590A80E1E.root',
	aodfiledir + 'C2E4E0B5-F176-E411-8525-D8D385FF4A72.root',
	aodfiledir + 'C42ABF54-EB76-E411-9397-002590A4C69A.root',
	aodfiledir + 'CEFB1297-F276-E411-9F2A-002590200900.root',
	aodfiledir + 'D4BF93D5-F076-E411-87DA-002590A831CC.root',
	aodfiledir + 'DC13DCE5-EB76-E411-BF43-001E67398223.root',
	aodfiledir + 'DC9DBCA5-E976-E411-AD32-0025B3E05BBE.root',
	aodfiledir + 'F6C064BF-3A77-E411-A171-001E6739677A.root',
	aodfiledir + 'F8EC1CC0-0377-E411-ADE9-001E673987D2.root',
	aodfiledir + 'FA84864E-F476-E411-B233-0025B3E063FA.root'
	),
	secondaryFileNames = cms.untracked.vstring(
	#fileNames = cms.untracked.vstring(
	#miniaod:
	miniaodfiledir + '1034F24B-1A78-E411-9D64-001E67398C1E.root',
	miniaodfiledir + '6856B40F-0C77-E411-893D-D8D385FF7678.root',
	miniaodfiledir + 'A8D5A514-DD77-E411-95AB-002481E14D64.root',
	miniaodfiledir + 'C20B68E7-0277-E411-85E5-001E67396A22.root',
	miniaodfiledir + 'E601D324-FA76-E411-B2A3-0025B3E066A4.root',
	miniaodfiledir + 'EC51B40A-0F77-E411-AB65-002590A831AA.root',
	miniaodfiledir + 'ECF2804E-F476-E411-A097-001E67398110.root'
	)
)

process.load("ttH-13TeVMultiLeptons.TemplateMakers.TriggerAna_cfi")


process.TriggerAna.triggers.hltlabel = "HLT" #"reHLT" #"HLT" # HLT = centrally produced samples
process.TriggerAna.triggers.trigger_vstring = "HLT_Mu13_Mu8_v23","HLT_Mu17_Mu8_v23","HLT_Mu17_TkMu8_v15","HLT_Mu22_TkMu8_v10","HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v20","HLT_Ele27_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele15_CaloIdT_CaloIsoVL_trackless_v9","HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v10","HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v10","HLT_Ele15_Ele8_Ele5_CaloIdL_TrkIdVL_v7" # triggers from the 2012 menu



######################################

	
process.TFileService = cms.Service("TFileService",
                                   #fileName = cms.string("test_100evts_muon_iso_study_" + str(looseMuonRelIso) + ".root") # name of output file
                                   fileName = cms.string("test_500evts_newtrig.root") # name of output file
)


process.p = cms.Path(process.TriggerAna)

# summary
process.options = cms.untracked.PSet(
	wantSummary = cms.untracked.bool(False),
	SkipEvent = cms.untracked.vstring('ProductNotFound')
)

