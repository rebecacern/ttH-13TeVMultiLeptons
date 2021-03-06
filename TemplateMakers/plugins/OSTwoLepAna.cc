// created by Geoff Smith

#include "ttH-13TeVMultiLeptons/TemplateMakers/interface/OSTwoLepAna.h"
#include "ttH-13TeVMultiLeptons/TemplateMakers/interface/EGammaMvaEleEstimatorFWLite.h"

OSTwoLepAna::OSTwoLepAna(const edm::ParameterSet& constructparams){ //Anything that needs to be done at creation time
	debug = constructparams.getParameter<bool> ("debug");
	entire_pset = constructparams;
	parse_params();


}
OSTwoLepAna::~OSTwoLepAna(){} //Anything that needs to be done at destruction time


void OSTwoLepAna::beginJob()
{


	ep = fopen ("ele_SS.txt", "w+");
	mp = fopen ("mu_SS.txt", "w+");
	
	// job setup
	
	sampleNumber = convertSampleNameToNumber(sampleName);
	SetUp(analysisYear, sampleNumber, analysisType::DIL, isData);
	SetFactorizedJetCorrector();
	setupMva();
	alltriggerstostudy = HLTInfo();
	
	// initialize some variables:
	electronTightID = electronID::electronTight;
	electronLooseID = electronID::electronLoose;
	electronPreselectedID = electronID::electronPreselection;
	muonTightID = muonID::muonTight;
	muonLooseID = muonID::muonLoose;
	muonPreselectedID = muonID::muonNoCuts;
	tauTightID = tauID::tauMedium;
	// tauLooseID = tauID::tauVLoose;
	tauPreselectedID = tauID::tauNonIso;
	
	// needed in edanalyzer:
	edm::Service<TFileService> newfs;
	
	// book histos:
	// numtighteles = 	newfs->make<TH1D>("numtighteles","numtighteles",6,0,6);
// 	numlooseeles = 	newfs->make<TH1D>("numlooseeles","numlooseeles",6,0,6);
// 	numtightmuons = newfs->make<TH1D>("numtightmuons","numtightmuons",6,0,6);
// 	numloosemuons = newfs->make<TH1D>("numloosemuons","numloosemuons",6,0,6);
// 	numrawjets = 	newfs->make<TH1D>("numrawjets","numrawjets",15,0,15);
// 	numjetsnoele = 			newfs->make<TH1D>("numjetsnoele","numjetsnoele",15,0,15);
// 	numjetsnomu = 			newfs->make<TH1D>("numjetsnomu","numjetsnomu",15,0,15);
// 	numjetsnomuorele = 		newfs->make<TH1D>("numjetsnomuorele","numjetsnomuorele",15,0,15);
// 	numjetscorrected = 		newfs->make<TH1D>("numjetscorrected","numjetscorrected",15,0,15);
// 	numjetsselectedloose = 		newfs->make<TH1D>("numjetsselectedloose","numjetsselectedloose",15,0,15);
// 	numbtagsselectedlooseCSVM = 	newfs->make<TH1D>("numbtagsselectedlooseCSVM","numbtagsselectedlooseCSVM",12,0,12);
// 	leadingtightmupt = 		newfs->make<TH1D>("leadingtightmupt","leadingtightmupt",200,0,400);
// 	subleadingtightmupt = 		newfs->make<TH1D>("subleadingtightmupt","subleadingtightmupt",200,0,400);
// 	leadingtightelept = 		newfs->make<TH1D>("leadingtightelept","leadingtightelept",200,0,400);
// 	subleadingtightelept = 		newfs->make<TH1D>("subleadingtightelept","subleadingtightelept",200,0,400);
// 	leadingtightleppt = 		newfs->make<TH1D>("leadingtightleppt","leadingtightleppt",200,0,400);
// 	subleadingtightleppt = 		newfs->make<TH1D>("subleadingtightleppt","subleadingtightleppt",200,0,400);
// 	jet1pt = newfs->make<TH1D>("jet1pt","jet1pt",200,0,400);
// 	jet2pt = newfs->make<TH1D>("jet2pt","jet2pt",200,0,400);
// 	jet3pt = newfs->make<TH1D>("jet3pt","jet3pt",200,0,400);
// 	jet4pt = newfs->make<TH1D>("jet4pt","jet4pt",200,0,400);   
// 	met_pt = newfs->make<TH1D>("met_pt","met_pt",200,0,400);
// 	lep1_lep2_pt = newfs->make<TH2D>("lep1_lep2_pt","lep1_lep2_pt",30,0,150,30,0,150);
// 	hlt_count_hist = newfs->make<TH1D>("hlt_count_hist","hlt_count_hist",1000,0,1000);
		
	// add the tree:
	summaryTree = newfs->make<TTree>("summaryTree", "Summary Event Values");	
	tree_add_branches();
	
	
}
void OSTwoLepAna::endJob() {
  

  fclose(ep);
  fclose(mp);
  //  cout << "Num Events processed " << numEvents << endl;
  //       << "Passed cuts " << numEventsPassCuts << endl;
  //       << "Failed cuts " << numEventsFailCuts << endl;
  
} // job completion (cutflow table, etc.)

void OSTwoLepAna::analyze(const edm::Event& event, const edm::EventSetup& evsetup) // this function is called once at each event
{
	// analysis goes here
	if (debug) cout << "event: " << event.id().event() << endl;
	

	eventcount++;
	SetupOptions(event);
  
	trigRes triggerResults = 		GetTriggers(event);
	patMuons muons = 			GetMuons(event);
	patJets pfjets = 			GetJets(event);
	patElectrons electrons = 		GetElectrons(event);
	patMETs mets = 				GetMet(event);
	prunedGenParticles prunedParticles = 	GetPrunedGenParticles(event);
	
	//patJets testHiggsjets = pfjets;
	
	SetRho(rho);
	
	int numpvs =				GetVertices(event);
	
	if (debug) cout << "numpvs: " << numpvs << endl;
		
	edm::Handle<GenEventInfoProduct> GenInfo;
    	event.getByLabel("generator",GenInfo);
    	
	///////////////////////////
	double mcwgt_intree = GenInfo->weight();		// <- gen-level weight
	double weight = 1.;					// <- analysis weight 
	weight *= mcwgt_intree;					// MC-only (flag to be added)
	///////////////////////////
	
	
	initialize_variables();	
			
	/////////
	///
	/// Electrons
	///
	////////
	
	//double minTightLeptonPt = 20.;
	//double minLooseLeptonPt = 10.;
	double minTightLeptonPt = 10.;
	double minLooseLeptonPt = 5.;

	double mintightelept = minTightLeptonPt;
	double minlooseelept = minLooseLeptonPt;

	//double mintightelept = 10.;
	//double minlooseelept = 5.;

	vecPatElectron selectedElectrons_tight = GetSelectedElectrons( *electrons, mintightelept, electronID::electronTight );	//miniAODhelper.
	vecPatElectron selectedElectrons_loose = GetSelectedElectrons( *electrons, minlooseelept, electronID::electronLoose );	//miniAODhelper.
	vecPatElectron selectedElectrons_preselected = GetSelectedElectrons( *electrons, 7., electronID::electronPreselection );	//miniAODhelper.
	vecPatElectron selectedElectrons_nocuts = GetSelectedElectrons( *electrons, 7., electronID::electronNoCuts );	//miniAODhelper.
	vecPatElectron selectedElectrons_forcleaning = GetSelectedElectrons( *electrons, 10., electronID::electronPreselection );	//miniAODhelper.
	vecPatElectron selectedElectrons_loose_notight = RemoveOverlaps( selectedElectrons_tight, selectedElectrons_loose);	//miniAODhelper.

	/////////
	///
	/// Muons
	///
	////////

	double mintightmupt = minTightLeptonPt;
	double minloosemupt = minLooseLeptonPt;

	vecPatMuon selectedMuons_tight = GetSelectedMuons( *muons, mintightmupt, muonID::muonTight );
	vecPatMuon selectedMuons_loose = GetSelectedMuons( *muons, minloosemupt, muonID::muonLoose );
	vecPatMuon selectedMuons_preselected = GetSelectedMuons( *muons, 5., muonID::muonPreselection );
	vecPatMuon selectedMuons_nocuts = GetSelectedMuons( *muons, 5., muonID::muonNoCuts );
	vecPatMuon selectedMuons_forcleaning = GetSelectedMuons( *muons, 10., muonID::muonPreselection );
	vecPatMuon selectedMuons_loose_notight = RemoveOverlaps(selectedMuons_tight,selectedMuons_loose);
	
	/////////
	///
	/// Leptons
	///
	////////
	
	selectedElectrons_preselected = cleanObjects<pat::Electron,pat::Muon>(selectedElectrons_preselected,selectedMuons_preselected,0.02); 	//remove electrons that are close (dR <=0.02) to muons
	vecPatLepton selectedLeptons_preselected = fillLeptons(selectedMuons_preselected,selectedElectrons_preselected);
	selectedLeptons_preselected = MiniAODHelper::GetSortedByPt(selectedLeptons_preselected);

	selectedElectrons_loose = cleanObjects<pat::Electron,pat::Muon>(selectedElectrons_loose,selectedMuons_loose,0.02);
	vecPatLepton selectedLeptons_loose = fillLeptons(selectedMuons_loose,selectedElectrons_loose);
	selectedLeptons_loose = MiniAODHelper::GetSortedByPt(selectedLeptons_loose);

	selectedElectrons_tight = cleanObjects<pat::Electron,pat::Muon>(selectedElectrons_tight,selectedMuons_tight,0.02);
	vecPatLepton selectedLeptons_tight = fillLeptons(selectedMuons_tight,selectedElectrons_tight);
	selectedLeptons_tight = MiniAODHelper::GetSortedByPt(selectedLeptons_tight);
	


	vecPatLepton selectedLeptons_nocuts = fillLeptons(selectedMuons_nocuts,selectedElectrons_nocuts);
	selectedLeptons_nocuts = MiniAODHelper::GetSortedByPt(selectedLeptons_nocuts);
	
	vecPatLepton selectedLeptons_forcleaning = fillLeptons(selectedMuons_forcleaning,selectedElectrons_forcleaning);
	
	/////////
	///
	/// Jets
	///
	////////
 	
	vecPatJet rawJets				= GetUncorrectedJets(*pfjets);  					  //miniAODhelper.
	std::vector<pat::Jet> cleaned_rawJets           = cleanObjects<pat::Jet,reco::LeafCandidate>(rawJets,selectedLeptons_forcleaning,0.4);
	vecPatJet jetsNoMu			       	= RemoveOverlaps(selectedMuons_loose, rawJets); 			    //miniAODhelper.
	vecPatJet jetsNoEle			       	= RemoveOverlaps(selectedElectrons_loose, rawJets);			    //miniAODhelper.
	vecPatJet jetsNoLep			       	= RemoveOverlaps(selectedElectrons_loose, jetsNoMu);			    //miniAODhelper.
	vecPatJet correctedJets_noSys		       	= GetCorrectedJets(jetsNoLep);  					    //miniAODhelper.
	vecPatJet selectedJets_noSys_unsorted	       	= GetSelectedJets(correctedJets_noSys, 30., 2.4, jetID::jetLoose, '-' );    //miniAODhelper.
	vecPatJet selectedJets_tag_noSys_unsorted	= GetSelectedJets(correctedJets_noSys, 30., 2.4, jetID::jetLoose, 'M' );   //miniAODhelper.
	vecPatJet selectedJets_loose_noSys_unsorted     = GetSelectedJets(correctedJets_noSys, 20., 2.4, jetID::jetLoose, '-' );    //miniAODhelper.
	vecPatJet selectedJets_loose_tag_noSys_unsorted	= GetSelectedJets(correctedJets_noSys, 20., 2.4, jetID::jetLoose, 'M' );   //miniAODhelper.
	vecPatJet selectedJets_bJetsLoose          	= GetSelectedJets(cleaned_rawJets, 25., 2.4, jetID::jetPU, 'L' );   //miniAODhelper.
	vecPatJet selectedJets_forSync          	= GetSelectedJets(cleaned_rawJets, 25., 2.4, jetID::jetPU, '-' );   //miniAODhelper.
	vecPatJet selectedJets_forLepMVA          	= GetSelectedJets(rawJets, 10., 2.4, jetID::none, '-' );   //miniAODhelper.
	
	// test
	vecPatJet *testHiggsjets  = &selectedJets_noSys_unsorted;
	TwoObjectKinematic<vecPatJet,vecPatJet> myNumHiggsLikeDijet15("mass", "num_within", "numHiggsLike_dijet_15_float", &(testHiggsjets), "jets_by_pt", 1, 99, &(testHiggsjets), "jets_by_pt", 1, 99, 115.0, "", "", 15.0);
	

	/////////
	///
	/// MET
	///
	////////

	//do anything to pat met here

	/////////////////////////
	//////
	////// fill the collections
	//////
	/////////////////////////

        vector<ttH::Lepton> preselected_leptons = GetCollection(selectedLeptons_preselected);
        vector<ttH::Lepton> loose_leptons = GetCollection(selectedLeptons_loose);
        vector<ttH::Lepton> tight_leptons = GetCollection(selectedLeptons_tight);

	vector<ttH::Electron> preselected_electrons = GetCollection(selectedElectrons_preselected,selectedJets_forLepMVA);
	vector<ttH::Electron> loose_electrons = GetCollection(selectedElectrons_loose,selectedJets_forLepMVA);
	vector<ttH::Electron> tight_electrons = GetCollection(selectedElectrons_tight,selectedJets_forLepMVA);

	vector<ttH::Muon> preselected_muons = GetCollection(selectedMuons_preselected,selectedJets_forLepMVA);
	vector<ttH::Muon> loose_muons = GetCollection(selectedMuons_loose,selectedJets_forLepMVA);
	vector<ttH::Muon> tight_muons = GetCollection(selectedMuons_tight,selectedJets_forLepMVA);

	vector<ttH::Jet> preselected_jets = GetCollection(selectedJets_forSync);
	vector<ttH::Jet> loose_bJets = GetCollection(selectedJets_bJetsLoose);

	vector<ttH::MET> theMET = GetCollection(mets);

	/////////////////////////
	//////
	////// cut flow studies
	//////
	/////////////////////////
	
	// if( int(selectedLeptons_preselected.size()) > 0)
	//   {
	//     cout << "leptons_preselected 0 pt " << preselected_leptons[0].obj.Pt() << endl;
	//   }
	
	//	cout << "num loose b jets " << bJets_loose.size() << endl;
	
	int higgs_daughter1 = GetHiggsDaughterId(*prunedParticles);
	//	int higgs_daughter2 = GetHiggsDaughterId(*prunedParticles,2);
	
	higgs_decay_intree = (higgs_daughter1==24 || higgs_daughter1==23 || higgs_daughter1==15) ? 1 : 0;
	
	eventnum_intree = event.id().event();
	lumiBlock_intree = event.id().luminosityBlock();
	runNumber_intree = event.id().run();
	
	// met_pt_intree = floor(theMET.Pt()*100+0.5)/100;
	// met_phi_intree = floor(theMET.Phi()*100+0.5)/100;
	
	// if (num_preselected_leptons_intree >=1 ){
	//   lep1_id_intree = selectedLeptons_preselected[0].pdgId();
	//   lep1_pt_intree = selectedLeptons_preselected[0].pt();
	//   lep1_eta_intree = selectedLeptons_preselected[0].eta();
	//   lep1_phi_intree = selectedLeptons_preselected[0].phi();
	// }
	
	// if (num_preselected_leptons_intree >=2 ){
	//   lep2_id_intree = selectedLeptons_preselected[1].pdgId();
	//   lep2_pt_intree = selectedLeptons_preselected[1].pt();
	//   lep2_eta_intree = selectedLeptons_preselected[1].eta();
	//   lep2_phi_intree = selectedLeptons_preselected[1].phi();
	// }
	
	
	// if (num_preselected_muons_intree >= 1)
	//   {
	//     mu1_charge_intree = selectedMuons_preselected[0].charge();
	//     mu1_pt_intree = selectedMuons_preselected[0].pt();
	//     mu1_eta_intree = selectedMuons_preselected[0].eta();
	//     mu1_phi_intree = selectedMuons_preselected[0].phi();
	//     mu1_id_intree = selectedMuons_preselected[0].pdgId();

	//     mu1_lepMVA_intree = GetMuonLepMVA(selectedMuons_preselected[0],selectedJets_forLepMVA);
	//     mu1_chargeFlip_intree = (selectedMuons_preselected[0].innerTrack()->ptError()/selectedMuons_preselected[0].innerTrack()->pt() < 0.2 ) ? 1 : 0;
	    
	//     mu1_chRelIso_intree = selectedMuons_preselected[0].chargedHadronIso()/selectedMuons_preselected[0].pt();
	//     mu1_nuRelIso_intree = GetMuonRelIsoR04(selectedMuons_preselected[0]) - mu1_chRelIso_intree;
	//     pat::Jet matchedJet = getClosestJet(selectedJets_forLepMVA,selectedMuons_preselected[0]);
	//     double dR = MiniAODHelper::DeltaR(&matchedJet,&selectedMuons_preselected[0]);
	//     mu1_jetdR_intree = min(dR,0.5);
	//     mu1_jetPtRatio_intree =  min(selectedMuons_preselected[0].pt()/matchedJet.pt(), float(1.5));
	//     mu1_bTagCSV_intree = max(matchedJet.bDiscriminator("combinedSecondaryVertexBJetTags"), float(0.0));
	//     mu1_sip3d_intree = fabs(selectedMuons_preselected[0].dB(pat::Muon::PV3D)/selectedMuons_preselected[0].edB(pat::Muon::PV3D));
	    
	//   } 
	
	// if (num_preselected_muons_intree >= 2)
	//   {
	//     mu2_eta_intree = selectedMuons_preselected[1].eta();
	//     mu2_phi_intree = selectedMuons_preselected[1].phi();
	//     mu2_id_intree = selectedMuons_preselected[1].pdgId();
	//     mu2_charge_intree = selectedMuons_preselected[1].charge();
        //     mu2_pt_intree = selectedMuons_preselected[1].pt();
        //     mu2_lepMVA_intree = GetMuonLepMVA(selectedMuons_preselected[1],selectedJets_forLepMVA);
        //     mu2_chargeFlip_intree = (selectedMuons_preselected[1].innerTrack()->ptError()/selectedMuons_preselected[1].innerTrack()->pt() < 0.2 ) ? 1 : 0;
	//   }

	

	// if (num_preselected_electrons_intree >= 1)
	//   {								

	//     ele1_eta_intree = selectedElectrons_preselected[0].eta();
	//     ele1_phi_intree = selectedElectrons_preselected[0].phi();
	//     ele1_id_intree = selectedElectrons_preselected[0].pdgId();

	//     ele1_charge_intree = selectedElectrons_preselected[0].charge();
	//     ele1_pt_intree = selectedElectrons_preselected[0].pt();
	//     ele1_lepMVA_intree = GetElectronLepMVA(selectedElectrons_preselected[0],selectedJets_forLepMVA);
	    
	//     bool ele1_chargeFlipA = selectedElectrons_preselected[0].isGsfCtfScPixChargeConsistent();
	//     //bool ele1_chargeFlipB = ( selectedElectrons_preselected[0].gsfTrack()->trackerExpectedHitsInner().numberOfHits() == 0 ); // 70X
	//     bool ele1_chargeFlipB = ( selectedElectrons_preselected[0].gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::TRACK_HITS) == 0 ); // 72X
	//     bool ele1_chargeFlipC = selectedElectrons_preselected[0].passConversionVeto();

	//     //bool ele2_chargeFlipA = selectedElectrons_preselected[1].isGsfCtfScPixChargeConsistent();
	//     ////bool ele2_chargeFlipB = (selectedElectrons_preselected[1].gsfTrack()->trackerExpectedHitsInner().numberOfHits() == 0);  // 70X
	//     //bool ele2_chargeFlipB = (selectedElectrons_preselected[1].gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::TRACK_HITS) == 0 ); // 72X
	//     //bool ele2_chargeFlipC = selectedElectrons_preselected[1].passConversionVeto();

	//     ele1_chargeFlip_intree =  (ele1_chargeFlipA && ele1_chargeFlipB && ele1_chargeFlipC) ? 1 : 0;
	    
	//     ele1_chRelIso_intree = selectedElectrons_preselected[0].chargedHadronIso()/selectedElectrons_preselected[0].pt();
	//     ele1_nuRelIso_intree = GetElectronRelIso(selectedElectrons_preselected[0]) - ele1_chRelIso_intree;
	//     pat::Jet matchedJet = getClosestJet(selectedJets_forLepMVA,selectedElectrons_preselected[0]);
	//     double dR = MiniAODHelper::DeltaR(&matchedJet,&selectedElectrons_preselected[0]);
	//     ele1_jetdR_intree = min(dR,0.5);
	//     ele1_jetPtRatio_intree =  min(selectedElectrons_preselected[0].pt()/matchedJet.pt(), float(1.5));
	//     ele1_bTagCSV_intree = max(matchedJet.bDiscriminator("combinedSecondaryVertexBJetTags"), float(0.0));
	//     ele1_sip3d_intree = fabs(selectedElectrons_preselected[0].dB(pat::Electron::PV3D)/selectedElectrons_preselected[0].edB(pat::Electron::PV3D));

	//   } 

	// if (num_preselected_electrons_intree >= 2)
	//   {
	//     bool ele2_chargeFlipA = selectedElectrons_preselected[1].isGsfCtfScPixChargeConsistent();
	//     //	    bool ele2_chargeFlipB = (selectedElectrons_preselected[1].gsfTrack()->trackerExpectedHitsInner().numberOfHits() == 0);
	//     bool ele2_chargeFlipB = (selectedElectrons_preselected[1].gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::TRACK_HITS) == 0 ); // 72X
	//     bool ele2_chargeFlipC = selectedElectrons_preselected[1].passConversionVeto();

	//     ele2_charge_intree = selectedElectrons_preselected[1].charge();
        //     ele2_pt_intree = selectedElectrons_preselected[1].pt();
        //     ele2_lepMVA_intree = GetElectronLepMVA(selectedElectrons_preselected[1],selectedJets_forLepMVA);
        //     ele2_chargeFlip_intree =  (ele2_chargeFlipA && ele2_chargeFlipB && ele2_chargeFlipC) ? 1 : 0;
	    
	//     ele2_eta_intree = selectedElectrons_preselected[1].eta();
	//     ele2_phi_intree = selectedElectrons_preselected[1].phi();
	//     ele2_id_intree = selectedElectrons_preselected[1].pdgId();

	//   }

	// if ( eventnum_intree == 156980)
	//   {
	//     for (unsigned int count = 0; count < selectedElectrons_nocuts.size(); ++count)
	//       {
	// 	cout << "event # " << eventnum_intree << endl;
	// 	cout << "Electron # " << count <<" Electron ID = " << selectedElectrons_nocuts[count].pdgId() <<endl;
	// 	cout <<"pt= "<<selectedElectrons_nocuts[count].pt()<<" eta= "<<selectedElectrons_nocuts[count].eta()<<" phi = "<<selectedLeptons_preselected[count].phi()<<endl;
	// 	cout << "higgs decay = " << higgs_decay_intree << endl;
	// 	cout << "decay type " << higgs_daughter1 << endl;
	// 	cout <<" dxy= "<< fabs(selectedElectrons_nocuts[count].gsfTrack()->dxy(vertex.position()))<<" dz = "<< fabs(selectedElectrons_nocuts[count].gsfTrack()->dz(vertex.position()))<<endl;
	// 	cout << "Super Cluster Eta = " << selectedElectrons_nocuts[count].superCluster()->position().eta() << endl;
	// 	cout <<"mvaID= "<< mvaID_->mvaValue(selectedElectrons_nocuts[count],vertex,rho,true,false) << endl;
	// 	cout << "ELE RelIso = " << GetElectronRelIso(selectedElectrons_nocuts[count]) << endl; 
	// 	cout << "lep MVA = " << GetElectronLepMVA(selectedElectrons_nocuts[count],selectedJets_forLepMVA) << endl;
	// 	// cout << "==== lep MVA Input variables ====" << endl;
	// 	// pat::Jet matchedJet = getClosestJet(selectedJets_forLepMVA,selectedElectrons_nocuts[count]);
	// 	// double dR = MiniAODHelper::DeltaR(&matchedJet,&selectedElectrons_nocuts[count]);
		
	// 	// cout << "neuRelIso = " << max(0.0,(selectedElectrons_nocuts[count].neutralHadronIso()+selectedElectrons_nocuts[count].photonIso())-0.5*selectedElectrons_nocuts[count].puChargedHadronIso())/selectedElectrons_nocuts[count].pt() << endl;
	// 	// cout << "chRelIso = " << selectedElectrons_nocuts[count].chargedHadronIso()/selectedElectrons_nocuts[count].pt() << endl;
	// 	// cout << "jetDR_in = " << min(dR,0.5) << endl;
	// 	// cout << "jetPtRatio_in = " << min(selectedElectrons_nocuts[count].pt()/matchedJet.pt(), float(1.5)) << endl;
	// 	// cout << "jetBTagCSV_in = " << max(matchedJet.bDiscriminator("combinedSecondaryVertexBJetTags"), float(0.0)) << endl;
	// 	// cout << "sip3d = " << fabs(selectedElectrons_nocuts[count].dB(pat::Electron::PV3D)/selectedElectrons_nocuts[count].edB(pat::Electron::PV3D)) << endl;
		
	// 	// for (unsigned int count = 0; count < selectedJets_bJetsLoose.size(); ++count)
	// 	//   {
	// 	//     cout << "=== " << count << " ===" << endl;
	// 	//     cout << "jet pt = " << selectedJets_bJetsLoose[count].pt() << endl;
	// 	//     cout << "jet eta = " << selectedJets_bJetsLoose[count].eta() << endl;
	// 	//     cout << "jet phi = " << selectedJets_bJetsLoose[count].phi() << endl;
	// 	//     cout << "jet CSV = " << selectedJets_bJetsLoose[count].bDiscriminator("combinedSecondaryVertexBJetTags") << endl;
	// 	//     cout << "  " << endl;
	// 	//   }
	//       }
	    
	//     for (unsigned int count = 0; count < selectedMuons_nocuts.size(); ++count)
	//       {
	// 	cout << "event # " << eventnum_intree << endl;
	// 	cout << "Muon # " << count <<" Muon ID = " << selectedMuons_nocuts[count].pdgId() <<endl;
	// 	cout <<"pt= "<<selectedMuons_nocuts[count].pt()<<" eta= "<<selectedMuons_nocuts[count].eta()<<" phi = "<<selectedLeptons_nocuts[count].phi()<<endl;
	// 	cout << "higgs decay = " << higgs_decay_intree << endl;
	// 	cout << "decay type "  << higgs_daughter1 << endl;
		
	// 	cout <<"reliso03= "<< GetMuonRelIsoR03(selectedMuons_nocuts[count]) << endl;
	// 	if( selectedMuons_nocuts[count].innerTrack().isAvailable() ){
	// 	  cout <<" dxy= "<< fabs(selectedMuons_nocuts[count].innerTrack()->dxy(vertex.position()))<<" dz = "<< fabs(selectedMuons_nocuts[count].innerTrack()->dz(vertex.position()))<<endl;
	// 	}
	// 	else {
	// 	  cout << "no muon inner track available " << endl;
	// 	}
		
	// 	cout << "PFmuon " << selectedMuons_nocuts[count].isPFMuon() <<endl;
	// 	cout << "Trackermuon " << selectedMuons_nocuts[count].isTrackerMuon() <<endl;
	// 	cout << "Globalmuon " << selectedMuons_nocuts[count].isGlobalMuon() <<endl;
		
	// 	cout << "lep MVA = " << GetMuonLepMVA(selectedMuons_nocuts[count],selectedJets_forLepMVA) << endl;
	// 	cout << "==== lep MVA Input variables ====" << endl;
	// 	pat::Jet matchedJet = getClosestJet(selectedJets_forLepMVA,selectedMuons_nocuts[count]);
	// 	double dR = MiniAODHelper::DeltaR(&matchedJet,&selectedMuons_nocuts[count]);
		
	// 	cout << "neuRelIso = " << max(0.0,(selectedMuons_nocuts[count].neutralHadronIso()+selectedMuons_nocuts[count].photonIso())-0.5*selectedMuons_nocuts[count].puChargedHadronIso())/selectedMuons_nocuts[count].pt() << endl;
	// 	cout << "chRelIso = " << selectedMuons_nocuts[count].chargedHadronIso()/selectedMuons_nocuts[count].pt() << endl;
	// 	cout << "jetDR_in = " << min(dR,0.5) << endl;
	// 	cout << "jetPtRatio_in = " << min(selectedMuons_nocuts[count].pt()/matchedJet.pt(), float(1.5)) << endl;
	// 	cout << "jetBTagCSV_in = " << max(matchedJet.bDiscriminator("combinedSecondaryVertexBJetTags"), float(0.0)) << endl;
	// 	cout << "sip3d = " << fabs(selectedMuons_nocuts[count].dB(pat::Muon::PV3D)/selectedMuons_nocuts[count].edB(pat::Muon::PV3D)) << endl;



	//       }
	    
	//   }
	

	
	// if (num_preselected_leptons_intree >= 2 && higgs_decay_intree == 1 && lep1_id_intree == lep2_id_intree && abs(lep1_id_intree) == 11 && ele1_lepMVA_intree > 0.7 && ele2_lepMVA_intree > 0.7)
	//   {
	//     fprintf(ep,"%6d %6d %10d  %+2d  %6.2f %+4.2f %+4.2f   %+2d  %6.2f %+4.2f %+4.2f    %6.1f  %+4.2f    %d \n",
	// 	    runNumber_intree, lumiBlock_intree, eventnum_intree,
	// 	    lep1_id_intree, lep1_pt_intree, lep1_eta_intree, lep1_phi_intree,
	// 	    lep2_id_intree, lep2_pt_intree, lep2_eta_intree, lep2_phi_intree,
	// 	    met_pt_intree, met_phi_intree,
	// 	    num_Jets_intree);
	//   }
	

	// if (num_preselected_leptons_intree >= 2 && higgs_decay_intree == 1 && lep1_id_intree == lep2_id_intree && abs(lep1_id_intree) == 13 && mu1_lepMVA_intree > 0.7 && mu2_lepMVA_intree > 0.7 && mu1_chargeFlip_intree ==1 && mu2_chargeFlip_intree == 1 && num_BJetsLoose_intree >=2)
	//   {
	//     fprintf(mp,"%6d %6d %10d  %+2d  %6.2f %+4.2f %+4.2f   %+2d  %6.2f %+4.2f %+4.2f    %6.1f  %+4.2f    %d \n",
	// 	    runNumber_intree, lumiBlock_intree, eventnum_intree,
	// 	    lep1_id_intree, lep1_pt_intree, lep1_eta_intree, lep1_phi_intree,
	// 	    lep2_id_intree, lep2_pt_intree, lep2_eta_intree, lep2_phi_intree,
	// 	    met_pt_intree, met_phi_intree,
	// 	    num_Jets_intree);
	//   }

	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// fill some basic histos:

// 	numtighteles->						Fill(numTightElectrons,weight);
// 	numlooseeles->						Fill(numLooseElectrons,weight);
// 	numtightmuons->						Fill(numTightMuons,weight);
// 	numloosemuons->						Fill(numLooseMuons,weight);
// 	numrawjets->						Fill(numRawJets,weight);
// 	numjetsnoele->						Fill(numJetsNoEle,weight);
// 	numjetsnomu->						Fill(numJetsNoMu,weight);
// 	numjetsnomuorele->					Fill(numJetsNoLep,weight);
// 	numjetscorrected->					Fill(numJetsCorr,weight);
// 	numjetsselectedloose->					Fill(numSelJetsLoose,weight);
// 	numbtagsselectedlooseCSVM->				Fill(numSelJetsLooseCSVM,weight);
// 	if (numTightMuons) leadingtightmupt->			Fill(muonsTLVtight[0].Pt(),weight);
// 	if (numTightMuons>1) subleadingtightmupt->		Fill(muonsTLVtight[1].Pt(),weight);
// 	if (numTightElectrons) leadingtightelept->		Fill(elesTLVtight[0].Pt(),weight);
// 	if (numTightElectrons>1) subleadingtightelept->		Fill(elesTLVtight[1].Pt(),weight);
// 	if (leptonsTLVtight.size()) leadingtightleppt->		Fill(leptonsTLVtight[0].Pt(),weight);
// 	if (leptonsTLVtight.size()>1) subleadingtightleppt->	Fill(leptonsTLVtight[1].Pt(),weight);
// 	if (jetsTLVloose.size())jet1pt->			Fill(jetsTLVloose[0].Pt(),weight);
// 	if (jetsTLVloose.size()>1)jet2pt->			Fill(jetsTLVloose[1].Pt(),weight);
// 	if (jetsTLVloose.size()>2)jet3pt->			Fill(jetsTLVloose[2].Pt(),weight);
// 	if (jetsTLVloose.size()>3)jet4pt->			Fill(jetsTLVloose[3].Pt(),weight);
// 	met_pt->						Fill(theMET.Pt(),weight);

	
// 	TwoMuon = 0; TwoElectron = 0; MuonElectron = 0; PassTwoLepton = 0; 
// 	if ( numTightMuons >= 1 && numTightLooseMuons == 2 && numTightLooseElectrons == 0 ) TwoMuon = 1; 
// 	if ( numTightElectrons >= 1 && numTightLooseElectrons == 2 && numTightLooseMuons == 0 ) TwoElectron = 1; 
// 	if ( ( numTightMuons + numTightElectrons ) >= 1 && numTightLooseMuons == 1 && numTightLooseElectrons == 1 ) MuonElectron = 1; 
// 	if ( TwoMuon + TwoElectron + MuonElectron == 1 ) PassTwoLepton = 1;
	
	
	myNumHiggsLikeDijet15.evaluate();
	if (myNumHiggsLikeDijet15.myVars.size()) NumHiggsLikeDijet15_intree =  myNumHiggsLikeDijet15.myVars[0].branchVal;
	
        preselected_leptons_intree = preselected_leptons;
        preselected_electrons_intree = preselected_electrons;
        preselected_muons_intree = preselected_muons;

        loose_leptons_intree = loose_leptons;
        loose_electrons_intree = loose_electrons;
        loose_muons_intree = loose_muons;

        tight_leptons_intree = tight_leptons;
        tight_electrons_intree = tight_electrons;
        tight_muons_intree = tight_muons;

	preselected_jets_intree = preselected_jets;
	loose_bJets_intree = loose_bJets;
	
	met_intree = theMET;

	wgt_intree = weight;
	
	// fill it:
	summaryTree->Fill();
	
} // end event loop

void OSTwoLepAna::beginRun(edm::Run const& run, edm::EventSetup const& evsetup)
{
		
	// This has to be done here (not at beginjob):
	
	bool changed = true;
	if (hltConfig_.init(run, evsetup, hltTag, changed)) std::cout << "HLT config with process name " << hltTag << " successfully extracted" << std::endl;
	
	else std::cout << "Warning, didn't find process " << hltTag << std::endl;
	
	std::cout << " HLTConfig processName " << hltConfig_.processName() << " tableName " << hltConfig_.tableName() << " size " << hltConfig_.size() << std::endl; // " globalTag: " << hltConfig_.globalTag() << std::endl;
	
}
void OSTwoLepAna::endRun(edm::Run const& run, edm::EventSetup const& evsetup)
{

	cout << "total events processed: " << eventcount << endl;
	
	

} // anything special for the end of a run

void OSTwoLepAna::beginLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& evsetup){} // anything special for the beginning of a lumi block
void OSTwoLepAna::endLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& evsetup){} // anything special for the end of a lumi block


DEFINE_FWK_MODULE(OSTwoLepAna);

//  LocalWords:  chargeFlipC
