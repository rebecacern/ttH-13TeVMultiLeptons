// created by Geoff Smith

#include "ttH-13TeVMultiLeptons/TemplateMakers/interface/TriggerAna.h"
#include "ttH-13TeVMultiLeptons/TemplateMakers/interface/EGammaMvaEleEstimatorFWLite.h"

	
TriggerAna::TriggerAna(const edm::ParameterSet& constructparams){ //Anything that needs to be done at creation time
	debug = constructparams.getParameter<bool> ("debug");
	entire_pset = constructparams;
	parse_params();
}
TriggerAna::~TriggerAna(){} //Anything that needs to be done at destruction time


void TriggerAna::beginJob()
{
	edm::Service<TFileService> newfs;
	
	sampleNumber=9120;
	ep = fopen ("ele_SS.txt", "w+");
	mp = fopen ("mu_SS.txt", "w+");
	
	SetUp(analysisYear, sampleNumber, analysisType::DIL, isData);
	SetFactorizedJetCorrector();
	setupMva();
	
	// book histos:

	numtighteles = 	newfs->make<TH1D>("numtighteles","numtighteles",6,0,6);
	numlooseeles = 	newfs->make<TH1D>("numlooseeles","numlooseeles",6,0,6);
	numtightmuons = newfs->make<TH1D>("numtightmuons","numtightmuons",6,0,6);
	numloosemuons = newfs->make<TH1D>("numloosemuons","numloosemuons",6,0,6);
	
	numtightleptons = newfs->make<TH1D>("numtightleptons","numtightleptons",12,0,12);
	numlooseleptons = newfs->make<TH1D>("numlooseleptons","numlooseleptons",12,0,12);
	
	
	
	numrawjets = 	newfs->make<TH1D>("numrawjets","numrawjets",15,0,15);

	numjetsnoele = 			newfs->make<TH1D>("numjetsnoele","numjetsnoele",15,0,15);
	numjetsnomu = 			newfs->make<TH1D>("numjetsnomu","numjetsnomu",15,0,15);
	numjetsnomuorele = 		newfs->make<TH1D>("numjetsnomuorele","numjetsnomuorele",15,0,15);
	numjetscorrected = 		newfs->make<TH1D>("numjetscorrected","numjetscorrected",15,0,15);
	numjetsselectedloose = 		newfs->make<TH1D>("numjetsselectedloose","numjetsselectedloose",15,0,15);
	numbtagsselectedlooseCSVM = 	newfs->make<TH1D>("numbtagsselectedlooseCSVM","numbtagsselectedlooseCSVM",12,0,12);
	leadingtightmupt = 		newfs->make<TH1D>("leadingtightmupt","leadingtightmupt",200,0,400);
	subleadingtightmupt = 		newfs->make<TH1D>("subleadingtightmupt","subleadingtightmupt",200,0,400);
	leadingtightelept = 		newfs->make<TH1D>("leadingtightelept","leadingtightelept",200,0,400);
	subleadingtightelept = 		newfs->make<TH1D>("subleadingtightelept","subleadingtightelept",200,0,400);
	leadingtightleppt = 		newfs->make<TH1D>("leadingtightleppt","leadingtightleppt",200,0,400);
	subleadingtightleppt = 		newfs->make<TH1D>("subleadingtightleppt","subleadingtightleppt",200,0,400);
	
	leadingloosemupt = 		newfs->make<TH1D>("leadingloosemupt","leadingloosemupt",200,0,400);
	subleadingloosemupt = 		newfs->make<TH1D>("subleadingloosemupt","subleadingloosemupt",200,0,400);
	leadinglooseelept = 		newfs->make<TH1D>("leadinglooseelept","leadinglooseelept",200,0,400);
	subleadinglooseelept = 		newfs->make<TH1D>("subleadinglooseelept","subleadinglooseelept",200,0,400);
	leadinglooseleppt = 		newfs->make<TH1D>("leadinglooseleppt","leadinglooseleppt",200,0,400);
	subleadinglooseleppt = 		newfs->make<TH1D>("subleadinglooseleppt","subleadinglooseleppt",200,0,400);
	
	genparticle_IDs = 		newfs->make<TH1D>("genparticle_IDs","genparticle_IDs",200,-100,100);		
	
	numraweles = 			newfs->make<TH1D>("numraweles","numraweles",10,0,10);
	numrawmuons = 			newfs->make<TH1D>("numrawmuons","numrawmuons",10,0,10);
	numrawleps = 			newfs->make<TH1D>("numrawleps","numrawleps",10,0,10);
	rawele1pt = 			newfs->make<TH1D>("rawele1pt","rawele1pt",200,0,400);
	rawele2pt = 			newfs->make<TH1D>("rawele2pt","rawele2pt",200,0,400);
	rawmu1pt = 			newfs->make<TH1D>("rawmu1pt","rawmu1pt",200,0,400);
	rawmu2pt = 			newfs->make<TH1D>("rawmu2pt","rawmu2pt",200,0,400);
	rawlep1pt = 			newfs->make<TH1D>("rawlep1pt","rawlep1pt",200,0,400);
	rawlep2pt = 			newfs->make<TH1D>("rawlep2pt","rawlep2pt",200,0,400);
	rawlep3ormorept = 		newfs->make<TH1D>("rawlep3ormorept","rawlep3ormorept",200,0,400);


	jet1pt = newfs->make<TH1D>("jet1pt","jet1pt",200,0,400);
	jet2pt = newfs->make<TH1D>("jet2pt","jet2pt",200,0,400);
	jet3pt = newfs->make<TH1D>("jet3pt","jet3pt",200,0,400);
	jet4pt = newfs->make<TH1D>("jet4pt","jet4pt",200,0,400);   

	met_pt = newfs->make<TH1D>("met_pt","met_pt",200,0,400);

	lep1_lep2_pt = newfs->make<TH2D>("lep1_lep2_pt","lep1_lep2_pt",30,0,150,30,0,150);

	hlt_count_hist = newfs->make<TH1D>("hlt_count_hist","hlt_count_hist",1000,0,1000);
	
	gen_ele_info_vs_hlt = newfs->make<TH2D>("gen_ele_info_vs_hlt","gen_ele_info_vs_hlt",4,0,4,3,0,3);
	gen_mu_info_vs_hlt = newfs->make<TH2D>("gen_mu_info_vs_hlt","gen_mu_info_vs_hlt",4,0,4,3,0,3);
	
	genstatusinfo = newfs->make<TH2D>("genstatusinfo","genstatusinfo",200,-100,100,200,-100,100);
	
	leadingEle_pat_gen_chosen_reso = newfs->make<TH1D>("leadingEle_pat_gen_chosen_reso","leadingEle_pat_gen_chosen_reso",1000,0,10);
	subleadingEle_pat_gen_chosen_reso = newfs->make<TH1D>("subleadingEle_pat_gen_chosen_reso","subleadingEle_pat_gen_chosen_reso",1000,0,10);
	leadingEle_pat_gen_dpt = newfs->make<TH1D>("leadingEle_pat_gen_pt","leadingEle_pat_gen_pt",200,-2,2);
	subleadingEle_pat_gen_dpt = newfs->make<TH1D>("subleadingEle_pat_gen_pt","subleadingEle_pat_gen_pt",200,-2,2);
	leadingEle_pat_gen_deta_dphi = newfs->make<TH2D>("leadingEle_pat_gen_eta_phi","leadingEle_pat_gen_eta_phi",100,-5,5,100,-5,5);
	subleadingEle_pat_gen_deta_dphi = newfs->make<TH2D>("subleadingEle_pat_gen_eta_phi","subleadingEle_pat_gen_eta_phi",100,-5,5,100,-5,5);
	
	leadingMuon_pat_gen_dpt = newfs->make<TH1D>("leadingMuon_pat_gen_pt","leadingMuon_pat_gen_pt",200,-2,2);
	subleadingMuon_pat_gen_dpt = newfs->make<TH1D>("subleadingMuon_pat_gen_pt","subleadingMuon_pat_gen_pt",200,-2,2);
	leadingMuon_pat_gen_deta_dphi = newfs->make<TH2D>("leadingMuon_pat_gen_eta_phi","leadingMuon_pat_gen_eta_phi",100,-5,5,100,-5,5);
	subleadingMuon_pat_gen_deta_dphi = newfs->make<TH2D>("subleadingMuon_pat_gen_eta_phi","subleadingMuon_pat_gen_eta_phi",100,-5,5,100,-5,5);
	
	
	
	
	
	

	// initialize some variables:

	electronTightID = electronID::electronTight;
	electronLooseID = electronID::electronLoose;
	electronPreselectedID = electronID::electronNoCuts;

	muonTightID = muonID::muonTight;
	muonLooseID = muonID::muonLoose;
	muonPreselectedID = muonID::muonNoCuts;

	tauTightID = tauID::tauMedium;
	// tauLooseID = tauID::tauVLoose;
	tauPreselectedID = tauID::tauNonIso;

	eventcount=0;


	doublemucount=0.;
	doublemucount2=0.;
	doublemucount3=0.;
	doublemucount4=0.;
	doubleelecount=0.;
	doubleelecount2=0.;
	muelecount=0.;
	elemucount=0.;	
	tripelcount=0.;
	
	for (int i=0; i<1000; i++)
	{
		//allcount[i] = 0.;
		
		allcount_mumu[i] = 0.;
		allcount_elel[i] = 0.;
		allcount_muel[i] = 0.;
		allcount_elmu[i] = 0.;
		
	}

	numpassedcuts=0;

	numpassedmumucuts=0.;
	numpassedelelcuts=0.;
	numpassedmuelcuts=0.;
	numpassedelmucuts=0.;
	
	doublemucount5=0;
	
	
	alltriggerstostudy = HLTInfo();
	
	int numtrigs = alltriggerstostudy.size();
	
	if (numtrigs) mumutriggerstostudy.push_back(alltriggerstostudy[0]);
	if (numtrigs>1) mumutriggerstostudy.push_back(alltriggerstostudy[1]);
	if (numtrigs>2) mumutriggerstostudy.push_back(alltriggerstostudy[2]);
	if (numtrigs>3) mumutriggerstostudy.push_back(alltriggerstostudy[3]);
	if (numtrigs>4) eleltriggerstostudy.push_back(alltriggerstostudy[4]);
	if (numtrigs>5) eleltriggerstostudy.push_back(alltriggerstostudy[5]);
	if (numtrigs>6) mueltriggerstostudy.push_back(alltriggerstostudy[6]);
	if (numtrigs>7) elmutriggerstostudy.push_back(alltriggerstostudy[7]);
	if (numtrigs>8) tripeltriggerstostudy.push_back(alltriggerstostudy[8]);
	
	
	for (int i=0; i<10; i++)
	{
		if (i<numtrigs)
		{
			lep1_pt_didpass_double_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep1_pt_didpass_double_lep_" + alltriggerstostudy[i]).c_str(),100,0,200);
			lep2_pt_didpass_double_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep2_pt_didpass_double_lep_" + alltriggerstostudy[i]).c_str(),100,0,200);
			lep1_pt_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep1_pt_didntpass_double_lep_but_passed_single_lep_" + alltriggerstostudy[i]).c_str(),100,0,200);
			lep2_pt_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep2_pt_didntpass_double_lep_but_passed_single_lep_" + alltriggerstostudy[i]).c_str(),100,0,200);
		
			lep1_eta_didpass_double_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep1_eta_didpass_double_lep_" + alltriggerstostudy[i]).c_str(),100,-3,3);
			lep2_eta_didpass_double_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep2_eta_didpass_double_lep_" + alltriggerstostudy[i]).c_str(),100,-3,3);
			lep1_eta_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep1_eta_didntpass_double_lep_but_passed_single_lep_" + alltriggerstostudy[i]).c_str(),100,-3,3);
			lep2_eta_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep2_eta_didntpass_double_lep_but_passed_single_lep_" + alltriggerstostudy[i]).c_str(),100,-3,3);
		
			lep1_phi_didpass_double_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep1_phi_didpass_double_lep_" + alltriggerstostudy[i]).c_str(),100,-3.5,3.5);
			lep2_phi_didpass_double_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep2_phi_didpass_double_lep_" + alltriggerstostudy[i]).c_str(),100,-3.5,3.5);
			lep1_phi_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep1_phi_didntpass_double_lep_but_passed_single_lep_" + alltriggerstostudy[i]).c_str(),100,-3.5,3.5);
			lep2_phi_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep2_phi_didntpass_double_lep_but_passed_single_lep_" + alltriggerstostudy[i]).c_str(),100,-3.5,3.5);
		
			lep1_iso_didpass_double_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep1_iso_didpass_double_lep_" + alltriggerstostudy[i]).c_str(),100,0,0.25);
			lep2_iso_didpass_double_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep2_iso_didpass_double_lep_" + alltriggerstostudy[i]).c_str(),100,0,0.25);
			lep1_iso_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep1_iso_didntpass_double_lep_but_passed_single_lep_" + alltriggerstostudy[i]).c_str(),100,0,0.25);
			lep2_iso_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(alltriggerstostudy[i].c_str(),("lep2_iso_didntpass_double_lep_but_passed_single_lep_" + alltriggerstostudy[i]).c_str(),100,0,0.25);
		
			

			lep1_pt_didpass_double_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep1_pt_didpass_double_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),("lep1_pt_didpass_double_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),100,0,200,100,0,200);
			lep2_pt_didpass_double_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep2_pt_didpass_double_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),("lep2_pt_didpass_double_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),100,0,200,100,0,200);
			lep1_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep1_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),("lep1_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),100,0,200,100,0,200);
			lep2_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep2_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),("lep2_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),100,0,200,100,0,200);

			lep1_eta_didpass_double_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep1_eta_didpass_double_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),("lep1_eta_didpass_double_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),100,-3,3,100,-3,3);
			lep2_eta_didpass_double_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep2_eta_didpass_double_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),("lep2_eta_didpass_double_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),100,-3,3,100,-3,3);
			lep1_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep1_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),("lep1_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),100,-3,3,100,-3,3);
			lep2_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep2_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),("lep2_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + alltriggerstostudy[i]).c_str(),100,-3,3,100,-3,3);	
			
		
		
		}
		else
		{
			ostringstream os;
			os << i << endl;
			
			lep1_pt_didpass_double_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep1_pt_didpass_double_lep_" + os.str()).c_str(),100,0,200);
			lep2_pt_didpass_double_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep2_pt_didpass_double_lep_" + os.str()).c_str(),100,0,200);
			lep1_pt_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep1_pt_didntpass_double_lep_but_passed_single_lep_" + os.str()).c_str(),100,0,200);
			lep2_pt_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep2_pt_didntpass_double_lep_but_passed_single_lep_" + os.str()).c_str(),100,0,200);
		
			lep1_eta_didpass_double_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep1_eta_didpass_double_lep_" + os.str()).c_str(),100,-3,3);
			lep2_eta_didpass_double_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep2_eta_didpass_double_lep_" + os.str()).c_str(),100,-3,3);
			lep1_eta_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep1_eta_didntpass_double_lep_but_passed_single_lep_" + os.str()).c_str(),100,-3,3);
			lep2_eta_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep2_eta_didntpass_double_lep_but_passed_single_lep_" + os.str()).c_str(),100,-3,3);
		
			lep1_phi_didpass_double_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep1_phi_didpass_double_lep_" + os.str()).c_str(),100,-3.5,3.5);
			lep2_phi_didpass_double_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep2_phi_didpass_double_lep_" + os.str()).c_str(),100,-3.5,3.5);
			lep1_phi_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep1_phi_didntpass_double_lep_but_passed_single_lep_" + os.str()).c_str(),100,-3.5,3.5);
			lep2_phi_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep2_phi_didntpass_double_lep_but_passed_single_lep_" + os.str()).c_str(),100,-3.5,3.5);
		
			lep1_iso_didpass_double_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep1_iso_didpass_double_lep_" + os.str()).c_str(),100,0,0.25);
			lep2_iso_didpass_double_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep2_iso_didpass_double_lep_" + os.str()).c_str(),100,0,0.25);
			lep1_iso_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep1_iso_didntpass_double_lep_but_passed_single_lep_" + os.str()).c_str(),100,0,0.25);
			lep2_iso_didntpass_double_lep_but_passed_single_lep[i] = newfs->make<TH1D>(os.str().c_str(),("lep2_iso_didntpass_double_lep_but_passed_single_lep_" + os.str()).c_str(),100,0,0.25);
		
		
			
			lep1_pt_didpass_double_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep1_pt_didpass_double_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),("lep1_pt_didpass_double_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),100,0,200,100,0,200);
			lep2_pt_didpass_double_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep2_pt_didpass_double_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),("lep2_pt_didpass_double_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),100,0,200,100,0,200);
			lep1_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep1_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),("lep1_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),100,0,200,100,0,200);
			lep2_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep2_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),("lep2_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),100,0,200,100,0,200);

			lep1_eta_didpass_double_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep1_eta_didpass_double_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),("lep1_eta_didpass_double_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),100,-3,3,100,-3,3);
			lep2_eta_didpass_double_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep2_eta_didpass_double_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),("lep2_eta_didpass_double_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),100,-3,3,100,-3,3);
			lep1_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep1_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),("lep1_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),100,-3,3,100,-3,3);
			lep2_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[i] = newfs->make<TH2D>(("lep2_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),("lep2_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y_" + os.str()).c_str(),100,-3,3,100,-3,3);	
			
			

		
		
		}
	}
	
	mumu_lep1_denom = newfs->make<TH1D>("mumu_lep1_denom","mumu_lep1_denom",100,0,200);
	mumu_lep2_denom = newfs->make<TH1D>("mumu_lep2_denom","mumu_lep2_denom",100,0,200);
	muel_lep1_denom = newfs->make<TH1D>("muel_lep1_denom","muel_lep1_denom",100,0,200);
	muel_lep2_denom = newfs->make<TH1D>("muel_lep2_denom","muel_lep2_denom",100,0,200);
	elel_lep1_denom = newfs->make<TH1D>("elel_lep1_denom","elel_lep1_denom",100,0,200);
	elel_lep2_denom = newfs->make<TH1D>("elel_lep2_denom","elel_lep2_denom",100,0,200);
	elmu_lep1_denom = newfs->make<TH1D>("elmu_lep1_denom","elmu_lep1_denom",100,0,200);
	elmu_lep2_denom = newfs->make<TH1D>("elmu_lep2_denom","elmu_lep2_denom",100,0,200);
	

	
	summaryTree = newfs->make<TTree>("summaryTree", "Summary Event Values");
	tree_add_branches();
	
	

	
}
void TriggerAna::endJob() {

//  cout << "Num Events processed " << numEvents << endl;
//       << "Passed cuts " << numEventsPassCuts << endl;
//       << "Failed cuts " << numEventsFailCuts << endl;

	fclose(ep);
	fclose(mp);       
} // job completion (cutflow table, etc.)

void TriggerAna::analyze(const edm::Event& event, const edm::EventSetup& evsetup) // this function is called once at each event
{

	if (debug) cout << "hey0" << endl;
	// analysis goes here
	if (debug) cout << "event: " << event.id().event() << endl;
	
	eventcount++;
	SetupOptions(event);
  
	trigRes triggerResults = 	GetTriggers(event);
	patMuons muons = 		GetMuons(event);
	patJets pfjets = 		GetJets(event);
	patElectrons electrons = 	GetElectrons(event);
	patMETs mets = 			GetMet(event);
	prunedGenParticles genparticles=GetPrunedGenParticles(event);
	SetRho(rho);
	
	int numpvs =			GetVertices(event);
	if (debug) cout << "numpvs: " << numpvs << endl;
		
	edm::Handle<GenEventInfoProduct> GenInfo;
    	event.getByLabel("generator",GenInfo);
    	double wgt = GenInfo->weight();		// <- gen-level weight
	
	double weight = wgt; // 1.;			// <- analysis weight 
	//  int numEvents = 0;
	//  int numEventsFailCuts = 0;
	//  int numEventsPassCuts = 0;
	//  int printEvery = 1;
  
	
	//wgt = weight; //test

	initialize_variables();	
	
	/////////
	///
	/// genparticles
	///
	////////
	
	//pdgId()
	//status()
	//pt()
	//eta()
	//phi()
	
	int genpartsize = genparticles->size();
	
	for (int i=0; i<genpartsize; i++)
	{	
		if (abs(genparticles->at(i).pdgId())==6) genparticle_IDs->Fill(genparticles->at(i).status());
		genstatusinfo->Fill(genparticles->at(i).pdgId(),genparticles->at(i).status());
	}
	
	
	if (debug) cout << "hey1" << endl;
	
	/////////
	///
	/// Electrons
	///
	////////
	
	
	//double minTightLeptonPt = 25.;  // usual
	double minTightLeptonPt = 15.; // when setting looseleptons = tightleptons in test below
	double minLooseLeptonPt = 15.; // usual
	
	//double minTightLeptonPt = 20.; // 8 TeV
	//double minLooseLeptonPt = 10.; // 8 TeV
	//double minLooseLeptonPt = 2.;
	
	// baseline (other cuts below)
	//double minTightLeptonPt = 10.;
	//double minLooseLeptonPt = 5.;



	double mintightelept = minTightLeptonPt;
	double minlooseelept = minLooseLeptonPt;

	//double mintightelept = 10.;
	//double minlooseelept = 5.;

	vecPatElectron selectedElectrons_tight = GetSelectedElectrons( *electrons, mintightelept, electronID::electronTight );	//miniAODhelper. // old
	vecPatElectron selectedElectrons_loose = GetSelectedElectrons( *electrons, minlooseelept, electronID::electronLoose );	//miniAODhelper. // old
	vecPatElectron selectedElectrons_preselected = GetSelectedElectrons( *electrons, 7., electronID::electronPreselection );	//miniAODhelper.
	vecPatElectron selectedElectrons_nocuts = GetSelectedElectrons( *electrons, 7., electronID::electronNoCuts );	//miniAODhelper.
	vecPatElectron selectedElectrons_forcleaning = GetSelectedElectrons( *electrons, 10., electronID::electronPreselection );	//miniAODhelper.
	vecPatElectron selectedElectrons_loose_notight = RemoveOverlaps( selectedElectrons_tight, selectedElectrons_loose);	//miniAODhelper.
	
	
	
	int numTightElectrons = int(selectedElectrons_tight.size());
	int numLooseElectrons = int(selectedElectrons_loose.size());
	
	
	//vecTLorentzVectorCMS elesTLVloose = Get_vecTLorentzVectorCMS(selectedElectrons_loose);
	vecTLorentzVectorCMS elesTLVtight = Get_vecTLorentzVectorCMS(selectedElectrons_tight);
	vecTLorentzVectorCMS elesTLVloose = elesTLVtight; // test
	numLooseElectrons = elesTLVloose.size(); // test
	vecTLorentzVectorCMS elesTLVloosenotight = Get_vecTLorentzVectorCMS(selectedElectrons_loose_notight);

	/////////
	///
	/// Muons
	///
	////////

	if (debug) cout << "hey2" << endl;

	//double mintightmupt = 10.;
	double mintightmupt = minTightLeptonPt;
	double minloosemupt = minLooseLeptonPt;

	vecPatMuon selectedMuons_tight = GetSelectedMuons( *muons, mintightmupt, muonID::muonTight );		//miniAODhelper.
	vecPatMuon selectedMuons_loose = GetSelectedMuons( *muons, minloosemupt, muonID::muonLoose );		//miniAODhelper.
	vecPatMuon selectedMuons_preselected = GetSelectedMuons( *muons, 5., muonID::muonPreselection );	//miniAODhelper.
	vecPatMuon selectedMuons_nocuts = GetSelectedMuons( *muons, 5., muonID::muonNoCuts );	//miniAODhelper.
	vecPatMuon selectedMuons_forcleaning = GetSelectedMuons( *muons, 10., muonID::muonPreselection );	//miniAODhelper.
	vecPatMuon selectedMuons_loose_notight = RemoveOverlaps(selectedMuons_tight,selectedMuons_loose);		//miniAODhelper.

	int numTightMuons = int(selectedMuons_tight.size());
	int numLooseMuons = int(selectedMuons_loose.size());	

	//vecTLorentzVectorCMS muonsTLVloose = Get_vecTLorentzVectorCMS(selectedMuons_loose);
	vecTLorentzVectorCMS muonsTLVtight = Get_vecTLorentzVectorCMS(selectedMuons_tight);
	vecTLorentzVectorCMS muonsTLVloose = muonsTLVtight; // test
	numLooseMuons = muonsTLVloose.size(); //test
	vecTLorentzVectorCMS muonsTLVloosenotight = Get_vecTLorentzVectorCMS(selectedMuons_loose_notight);		  
			  
	
	if (debug) cout << "hey3" << endl;
	/////////
	///
	/// Leptons
	///
	////////
	
	if (debug) cout << "eles preselected: " << selectedElectrons_preselected.size() << " muons preselected: " << selectedMuons_preselected.size() << endl;
	
	
	//remove electrons that are close (dR <=0.02) to muons		  
	selectedElectrons_preselected = cleanObjects<pat::Electron,pat::Muon>(selectedElectrons_preselected,selectedMuons_preselected,0.02);
	
	// fill leptons
	vecPatLepton selectedLeptons_preselected = fillLeptons(selectedMuons_preselected,selectedElectrons_preselected);
	selectedLeptons_preselected = MiniAODHelper::GetSortedByPt(selectedLeptons_preselected);
	
	vecPatLepton selectedLeptons_nocuts = fillLeptons(selectedMuons_nocuts,selectedElectrons_nocuts);
	selectedLeptons_nocuts = MiniAODHelper::GetSortedByPt(selectedLeptons_nocuts);
	
	vecPatLepton selectedLeptons_forcleaning = fillLeptons(selectedMuons_forcleaning,selectedElectrons_forcleaning);		  



	// Geoff
	//vecTLorentzVectorCMS leptonsTLVtight = Get_vecTLorentzVectorCMS_sorted_leptons(muonsTLVtight, elesTLVtight); 
	//vecTLorentzVectorCMS leptonsTLVloose = Get_vecTLorentzVectorCMS_sorted_leptons(muonsTLVloose, elesTLVloose); 
	
	vdouble muonIsosLoose = Get_Isos(selectedMuons_loose);
	vdouble muonIsosTight = Get_Isos(selectedMuons_tight);
	vdouble electronIsosLoose = Get_Isos(selectedElectrons_loose);
	vdouble electronIsosTight = Get_Isos(selectedElectrons_tight);
	if (debug) cout << "eles preselected: " << selectedElectrons_preselected.size() << " muons preselected: " << selectedMuons_preselected.size() << endl;

	if (debug) cout << "hey4" << endl;

	
		
	/////////
	///
	/// Jets
	///
	////////

 	
	vecPatJet rawJets				= GetUncorrectedJets(*pfjets);  					  //miniAODhelper.
	vecPatJet cleaned_rawJets           		= cleanObjects<pat::Jet,reco::LeafCandidate>(rawJets,selectedLeptons_forcleaning,0.4);
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
	
	vecTLorentzVectorCMS jetsTLVloose = Get_vecTLorentzVectorCMS(selectedJets_loose_noSys_unsorted);
	vecTLorentzVectorCMS jetsTLVtight = Get_vecTLorentzVectorCMS(selectedJets_noSys_unsorted);
	
	vint vecJetPdgIDs = Get_JetPartonFlavor(selectedJets_loose_noSys_unsorted);

	int numRawJets = rawJets.size();
	int numJetsNoEle = jetsNoEle.size();
	int numJetsNoMu = jetsNoMu.size();
	int numJetsNoLep = jetsNoLep.size();
	int numJetsCorr = correctedJets_noSys.size();

	int numSelJetsLoose = selectedJets_loose_noSys_unsorted.size();
	int numSelJetsLooseCSVM = selectedJets_loose_tag_noSys_unsorted.size();

	
	if (debug) cout << "hey5" << endl;
	
	/////////
	///
	/// MET
	///
	////////
	
	TLorentzVectorCMS theMET = Get_TLorentzVectorCMS(mets);
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	///// doing this here instead of above .... note that you are changing the loose, tight vecTLV's
	
	vecPatElectron selectedElectrons_preselected_pass_tight_mva = Get_vecPatElectron_Passing_ElectronLepMVA(selectedElectrons_preselected,selectedJets_forLepMVA,0.7); // tight=0.7, loose=-0.3
	vecPatElectron selectedElectrons_preselected_pass_loose_mva = Get_vecPatElectron_Passing_ElectronLepMVA(selectedElectrons_preselected,selectedJets_forLepMVA,0.7); // tight=0.7, loose=-0.3
	
// 	elesTLVloose.clear();
// 	elesTLVtight.clear();
// 	//elesTLVloose = Get_vecTLorentzVectorCMS(selectedElectrons_preselected);
// 	elesTLVloose = Get_vecTLorentzVectorCMS(selectedElectrons_preselected_pass_loose_mva);
// 	elesTLVtight = Get_vecTLorentzVectorCMS(selectedElectrons_preselected_pass_tight_mva);
	
	vecPatMuon selectedMuons_preselected_pass_tight_mva = Get_vecPatMuon_Passing_MuonLepMVA(selectedMuons_preselected,selectedJets_forLepMVA,0.7); // tight=0.7, loose=-0.3
	vecPatMuon selectedMuons_preselected_pass_loose_mva = Get_vecPatMuon_Passing_MuonLepMVA(selectedMuons_preselected,selectedJets_forLepMVA,0.7); // tight=0.7, loose=-0.3
	
// 	muonsTLVloose.clear();
//  	muonsTLVtight.clear();
// 	//muonsTLVloose = Get_vecTLorentzVectorCMS(selectedMuons_preselected);
// 	muonsTLVloose = Get_vecTLorentzVectorCMS(selectedMuons_preselected_pass_loose_mva);
// 	muonsTLVtight = Get_vecTLorentzVectorCMS(selectedMuons_preselected_pass_tight_mva);
		
	vecTLorentzVectorCMS leptonsTLVtight = Get_vecTLorentzVectorCMS_sorted_leptons(muonsTLVtight, elesTLVtight);
	vecTLorentzVectorCMS leptonsTLVloose = Get_vecTLorentzVectorCMS_sorted_leptons(muonsTLVloose, elesTLVloose);
	
// 	numTightMuons = muonsTLVtight.size();
// 	numLooseMuons = muonsTLVloose.size();
// 	numTightElectrons = elesTLVtight.size();
// 	numLooseElectrons = elesTLVloose.size();
// 	
// 		
// 	muonIsosLoose = Get_Isos(selectedMuons_preselected_pass_loose_mva);
// 	muonIsosTight = Get_Isos(selectedMuons_preselected_pass_tight_mva);
// 	electronIsosLoose = Get_Isos(selectedElectrons_preselected_pass_loose_mva);
// 	electronIsosTight = Get_Isos(selectedElectrons_preselected_pass_tight_mva);

	
	
	if (debug) cout << "hey6" << endl;
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// assign values to tree variables:
	numLooseMuons_intree = numLooseMuons;
	numLooseElectrons_intree = numLooseElectrons;
	numTightMuons_intree = numTightMuons;
	numTightElectrons_intree = numTightElectrons;
	
	//testvect.push_back(3.);
	//testvect.push_back(11.);	
	//testTLV_intree = theMET;
	
	mcwgt_intree = wgt;
	
	jetpartonflavor_intree = vecJetPdgIDs;
	
	Jets_intree = jetsTLVloose;
	MET_intree = theMET;
	LooseElectrons_intree = elesTLVloose;
	LooseMuons_intree = muonsTLVloose;
	TightElectrons_intree = elesTLVtight;
	TightMuons_intree = muonsTLVtight;
	JetCSV_intree = ReturnBTagDisc(selectedJets_loose_noSys_unsorted);
	pileup_jet_ID_intree = ReturnPUJetID(selectedJets_loose_noSys_unsorted);
	
	
	// fill it:
	//summaryTree->Fill();
	
	if (debug) cout << "hey7" << endl;
	
	if ((numTightMuons+numTightElectrons)>0 && (numLooseMuons+numLooseElectrons)>1)
	{
	
		//////////////////////////////////////////////////////////////////////////////////////////////
		// fill some basic histos:

		numtighteles->Fill					(numTightElectrons,weight);
		numlooseeles->Fill					(numLooseElectrons,weight);
		numtightmuons->Fill					(numTightMuons,weight);
		numloosemuons->Fill					(numLooseMuons,weight);
		numtightleptons->Fill					(numTightMuons+numTightElectrons,weight);
		numlooseleptons->Fill					(numLooseMuons+numLooseElectrons,weight);
		numrawjets->Fill					(numRawJets,weight);
		numjetsnoele->Fill					(numJetsNoEle,weight);
		numjetsnomu->Fill					(numJetsNoMu,weight);
		numjetsnomuorele->Fill					(numJetsNoLep,weight);
		numjetscorrected->Fill					(numJetsCorr,weight);
		numjetsselectedloose->Fill				(numSelJetsLoose,weight);
		numbtagsselectedlooseCSVM->Fill				(numSelJetsLooseCSVM,weight);
		if (numTightMuons) leadingtightmupt->Fill		(muonsTLVtight[0].Pt(),weight);
		if (numTightMuons>1) subleadingtightmupt->Fill		(muonsTLVtight[1].Pt(),weight);
		if (numTightElectrons) leadingtightelept->Fill		(elesTLVtight[0].Pt(),weight);
		if (numTightElectrons>1) subleadingtightelept->Fill	(elesTLVtight[1].Pt(),weight);
		if (leptonsTLVtight.size()) leadingtightleppt->Fill	(leptonsTLVtight[0].Pt(),weight);
		if (leptonsTLVtight.size()>1) subleadingtightleppt->Fill(leptonsTLVtight[1].Pt(),weight);
		if (jetsTLVloose.size())jet1pt->Fill			(jetsTLVloose[0].Pt(),weight);
		if (jetsTLVloose.size()>1)jet2pt->Fill			(jetsTLVloose[1].Pt(),weight);
		if (jetsTLVloose.size()>2)jet3pt->Fill			(jetsTLVloose[2].Pt(),weight);
		if (jetsTLVloose.size()>3)jet4pt->Fill			(jetsTLVloose[3].Pt(),weight);
		met_pt->Fill						(theMET.Pt(),weight);
		if (numLooseMuons) leadingloosemupt->Fill		(muonsTLVloose[0].Pt(),weight);
		if (numLooseMuons>1) subleadingloosemupt->Fill		(muonsTLVloose[1].Pt(),weight);
		if (numLooseElectrons) leadinglooseelept->Fill		(elesTLVloose[0].Pt(),weight);
		if (numLooseElectrons>1) subleadinglooseelept->Fill	(elesTLVloose[1].Pt(),weight);
		if (leptonsTLVloose.size()) leadinglooseleppt->Fill	(leptonsTLVloose[0].Pt(),weight);
		if (leptonsTLVloose.size()>1) subleadinglooseleppt->Fill(leptonsTLVloose[1].Pt(),weight);



		numraweles->Fill  					(electrons->size(),weight);
		numrawmuons->Fill  					(muons->size(),weight);
		numrawleps->Fill  					(muons->size() + electrons->size(),weight);
		double ele1pt=0., ele2pt=0., mu1pt=0., mu2pt=0.;
		if (electrons->size()) ele1pt = electrons->at(0).pt();
		if (electrons->size()>1) ele2pt = electrons->at(1).pt();
		if (muons->size()) mu1pt = muons->at(0).pt();
		if (muons->size()>1) mu2pt = muons->at(1).pt();
		if (ele1pt) rawele1pt->Fill  				(ele1pt,weight);
		if (ele2pt) rawele2pt->Fill  				(ele2pt,weight);
		if (mu1pt) rawmu1pt->Fill  				(mu1pt,weight);
		if (mu2pt) rawmu2pt->Fill  				(mu2pt,weight);
		if (mu1pt + ele1pt) rawlep1pt->Fill			(std::max(mu1pt,ele1pt),weight);
		if (mu2pt + ele2pt) rawlep2pt->Fill			(std::max(mu2pt,ele2pt),weight);

		for (int i=2; i<(int)electrons->size(); i++) rawlep3ormorept->Fill(electrons->at(i).pt(),weight);
		for (int i=2; i<(int)muons->size(); i++) rawlep3ormorept->Fill(muons->at(i).pt(),weight);
	 
	}

	// .....


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Do anything else specific to the analysis (like apply event selection, evaluate variables, loop over systematics, save things to a tree)
	// In this case, I'm doing some trigger studies:
	// ////////////////////////////////////////////////////////////////////////////////////////////
	
	if (debug) cout << "hey8" << endl;

	
	edm::Handle<trigger::TriggerEvent> aodTriggerEvent;   
	event.getByLabel(edm::InputTag("hltTriggerSummaryAOD", "", hltTag), aodTriggerEvent); // this won't be in a regular miniAOD sample!
	const trigger::TriggerObjectCollection objects = aodTriggerEvent->getObjects();
	
	if (numLooseMuons>1)
	{
		for (int i=0; i<(numLooseMuons-1); i++)
		{
			if (muonsTLVloose[i+1].Pt()>muonsTLVloose[i].Pt()) cout << "hey!" << endl; // are they sorted by pt? yes.
		}
	}
	
	if (!triggerResults.isValid())
	{
		std::cout << "Trigger results not valid" << std::endl;
	}

	std::vector<std::string> myTriggernames = hltConfig_.triggerNames();
	
	int triggersize = myTriggernames.size();

	for (int j=0; j<triggersize; j++)
	{	
		unsigned int path__index = hltConfig_.triggerIndex(myTriggernames[j]);
	
		if (eventcount==1)
		{
			cout << myTriggernames[j] << endl;
		}
	
		if(triggerResults->accept(path__index))
		{
			hlt_count_hist->Fill(j);
		}
	}



	
	if (leptonsTLVloose.size()>1)
	{
		for (int i=0; i<30; i++)
		{
			for (int j=0; j<30; j++)
			{
				double amount = 5.;
				
				double leadingcut = (double)i*amount;
				double subleadingcut = (double)j*amount;
				
				if (leptonsTLVloose[0].Pt()>leadingcut&&leptonsTLVloose[1].Pt()>subleadingcut) lep1_lep2_pt->Fill(leadingcut,subleadingcut);
			}
		}
	}
	




	if (debug) cout << "hey9" << endl;


////////////////////// getting hlt objects from aodTriggerEvent:///////////////////////////////////////////
	
	strcpy (last_module_with_saved_tags_label,"none");
	
	int last_module_run_index = triggerResults->index(hltConfig_.triggerIndex(eleltriggerstostudy[0])); // NOT nec. same as last saved tags module
	
	vstring the_saved_tag_modules = hltConfig_.saveTagsModules(hltConfig_.triggerIndex(eleltriggerstostudy[0]));
	
	string mod_label = hltConfig_.moduleLabel(hltConfig_.triggerIndex(eleltriggerstostudy[0]),last_module_run_index);
	
 	vint last_module_with_saved_tags_obj_ID;
 	vdouble last_module_with_saved_tags_obj_pt;
 	vdouble last_module_with_saved_tags_obj_eta;
 	vdouble last_module_with_saved_tags_obj_phi;
 	int objs_found = 0;
	//char dummy = "";
	//char last_module_with_saved_tags_label[] = "";
	//int last_module_with_saved_tags_index;
	
	if (the_saved_tag_modules.size()>0)
	{
		for (int mods = (the_saved_tag_modules.size() - 1); mods>=0; mods--) // loop backwards through the modules
		{
			edm::InputTag moduleWhoseResultsWeWant(the_saved_tag_modules[mods], "", hltTag); // get the input tag of this module
						
			strcpy (last_module_with_saved_tags_label,the_saved_tag_modules[mods].c_str()); // get the name (char array) of this module				
			
			last_module_with_saved_tags_index = hltConfig_.moduleIndex(hltConfig_.triggerIndex(eleltriggerstostudy[0]),the_saved_tag_modules[mods]); // get this index of this module

			unsigned int indexOfModuleInAodTriggerEvent = aodTriggerEvent->filterIndex(moduleWhoseResultsWeWant); // get the index as saved in aodtriggerevent
			
			if (indexOfModuleInAodTriggerEvent < aodTriggerEvent->sizeFilters())
			{
				const trigger::Keys &keys = aodTriggerEvent->filterKeys( indexOfModuleInAodTriggerEvent ); // get the keys to the objects
				
				if (objs_found<(int)keys.size()) objs_found = keys.size(); // the number of saved objects in this module is == the size of the keys
				
				//if (objs_found>1)
				//{	
				
					for ( size_t iKey = 0; iKey < keys.size(); iKey++ )
					{
						trigger::TriggerObject foundObject = objects[keys[iKey]];					

						//int object_number = iKey;

						last_module_with_saved_tags_obj_ID.push_back(foundObject.id()); // save the object(s) info
						last_module_with_saved_tags_obj_pt.push_back(foundObject.pt());
						last_module_with_saved_tags_obj_eta.push_back(foundObject.eta());
						last_module_with_saved_tags_obj_phi.push_back(foundObject.phi());

					}

					if (last_module_with_saved_tags_obj_ID.size())
					{
						cout << last_module_with_saved_tags_label << endl;
						break;
					} // once ANY objects are found, break the loop (we want the objects of the last filter module that had saved object info, and then stop once we get them)
			
				//}
			}
		}
	}

// 	if (objs_found)
// 	{
// 		cout << " " << endl;
// 		cout << "the found objects: " << endl;
// 		
// 		for (int obit=0; obit<(int)last_module_with_saved_tags_obj_ID.size(); obit++)
// 		{
// 			cout << "  " << endl;
// 			cout << last_module_with_saved_tags_obj_ID[obit] << endl;
// 			cout << last_module_with_saved_tags_obj_pt[obit] << endl;
// 			cout << last_module_with_saved_tags_obj_eta[obit] << endl;
// 			cout << last_module_with_saved_tags_obj_phi[obit] << endl;
// 			cout << "  " << endl;
// 			cout << "  " << endl;
// 						
// 			
// 		}
// 	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	if (debug) cout << "hey10" << endl;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// select events with one tight lepton with pt > 20, and one additional loose lepton with pt >10 :
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//double leg1 = 20.; //pt	// 2012 menu
	//double leg2 = 10.; //pt	// 2012 menu
	
	double leg1 = 25.; //pt		// new 1.4 menu (electrons)
	double leg2 = 15.; //pt	// new 1.4 menu (electrons)
	//double leg2 = 10.; //pt		// trying something
	
	double loosedubmu2pt = -1.;
	double loosedubel2pt = -1.;
	
	double loosedubmu2eta = -99.;
	double loosedubel2eta = -99.;
	
	double loosedubmu2phi = -99.;
	double loosedubel2phi = -99.;
	
	double loosedubmu2iso = -99.;
	double loosedubel2iso = -99.;
	
	
	
	bool tightmupass = false;
	if ((numTightMuons>0)&&(muonsTLVtight[0].Pt()>leg1)) tightmupass = true;
	
	bool tightelepass = false;
	if ((numTightElectrons>0)&&(elesTLVtight[0].Pt()>leg1)) tightelepass = true;
	
	bool loosedubmupass = false;
	if (tightmupass&&(numLooseMuons>1))
	{
		if ((muonsTLVtight[0].Pt()!=muonsTLVloose[0].Pt())&&(muonsTLVloose[0].Pt()>leg2))
		{
			loosedubmupass=true; 
			loosedubmu2pt = muonsTLVloose[0].Pt(); 
			loosedubmu2eta = muonsTLVloose[0].Eta();
			loosedubmu2phi = muonsTLVloose[0].Phi();
			loosedubmu2iso = muonIsosLoose[0];
		}
		if ((muonsTLVtight[0].Pt()==muonsTLVloose[0].Pt())&&(muonsTLVloose[1].Pt()>leg2)) 
		{
			loosedubmupass=true; 
			loosedubmu2pt = muonsTLVloose[1].Pt(); 
			loosedubmu2eta = muonsTLVloose[1].Eta();
			loosedubmu2phi = muonsTLVloose[1].Phi();
			loosedubmu2iso = muonIsosLoose[1];
		}
	}
	
	bool loosedubelepass = false;
	if (tightelepass&&(numLooseElectrons>1))
	{
		if ((elesTLVtight[0].Pt()!=elesTLVloose[0].Pt())&&(elesTLVloose[0].Pt()>leg2)) 
		{ 
			loosedubelepass=true; 
			loosedubel2pt = elesTLVloose[0].Pt();
			loosedubel2eta = elesTLVloose[0].Eta();
			loosedubel2phi = elesTLVloose[0].Phi();
			loosedubel2iso = electronIsosLoose[0];
		}
		if ((elesTLVtight[0].Pt()==elesTLVloose[0].Pt())&&(elesTLVloose[1].Pt()>leg2)) 
		{ 
			loosedubelepass=true; 
			loosedubel2pt = elesTLVloose[1].Pt(); 
			loosedubel2eta = elesTLVloose[1].Eta();
			loosedubel2phi = elesTLVloose[1].Phi();
			loosedubel2iso = electronIsosLoose[1];
		}
	}
	
	bool muele_looseelepass = false;
	if (numLooseElectrons) if (tightmupass&&(elesTLVloose[0].Pt()>leg2)) muele_looseelepass = true;
	
	bool muele_loosemupass = false;
	if (numLooseMuons) if (tightelepass&&(muonsTLVloose[0].Pt()>leg2)) muele_loosemupass = true;
	
	
	
	//// out of those events, find which ones passed which triggers:
	// also, pt of leps that passed, didn't pass certain triggers:
	
	//string extra_trig = "HLT_IsoMu30_v12";
	//string extra_trig = "HLT_Ele27_WP80_v13";
	//extra_trig = "HLT_Ele27_eta2p1_WP85_Gsf_v1";
	extra_trig = alltriggerstostudy[9];
	//int extratrigindex = hltConfig_.triggerIndex(extra_trig);

	
	// find gen electrons:
	
	int gen_ele_count = 0;
	//bool foundgenleading = false;
	//bool foundgensubleading = false;
	int leadinggenindex = -1;
	int subleadinggenindex = -1;
	
	//double matchedpt = -99.
	//double matchedeta = -999.
	//double matchedphi = -999.
	double minamount = 99999.;
	
/////////// Electrons //////////////////////////	
	for (int i=0; i<genpartsize; i++)
	{		
		if (!tightelepass) continue;
		//if (!((genparticles->at(i).status()>20)&&(genparticles->at(i).status()<30))) continue; // if you include 0-10, much more electrons pass this whole thing. but, 20-30 is supposed to be hard scatter process...
		if (!((genparticles->at(i).status()>0)&&(genparticles->at(i).status()<10))) continue; // after looking at print statements, determined that 20-30 range is copy of some particles in 1-9 range
		//cout << "hey1" << endl;
		if (abs(genparticles->at(i).pdgId())!=11) continue;
		//cout << "hey2" << endl;
		//if (fabs(genparticles->at(i).eta())>2.5) continue;
		//cout << "hey3" << endl;
		if ( !(abs(genparticles->at(i).mother()->pdgId())==25 || abs(genparticles->at(i).mother()->pdgId())==24 || abs(genparticles->at(i).mother()->pdgId())==23) ) continue;
		
		gen_ele_count++; // trying something
		
		double dR = sqrt((elesTLVtight[0].Eta() - genparticles->at(i).eta())*(elesTLVtight[0].Eta() - genparticles->at(i).eta()) + (elesTLVtight[0].Phi() - genparticles->at(i).phi())*(elesTLVtight[0].Phi() - genparticles->at(i).phi()));
		double dptoverpt = fabs((elesTLVtight[0].Pt() - genparticles->at(i).pt()) / elesTLVtight[0].Pt());
		//double amount = dR + dptoverpt;
		double amount = dptoverpt;
		
		//if (amount<minamount)
		if (amount<minamount && dR<0.5)
		{
			minamount = amount;
			leadinggenindex = i;
		}
		
	}
	
	double minamount2 = 99999.;
	
	for (int i=0; i<genpartsize; i++)
	{		
		if (!loosedubelepass) continue;
		if (!((genparticles->at(i).status()>0)&&(genparticles->at(i).status()<10))) continue;
		if (i==leadinggenindex) continue;
		if (abs(genparticles->at(i).pdgId())!=11) continue;
		if ( !(abs(genparticles->at(i).mother()->pdgId())==25 || abs(genparticles->at(i).mother()->pdgId())==24 || abs(genparticles->at(i).mother()->pdgId())==23) ) continue;
		
		double dR = sqrt((loosedubel2eta - genparticles->at(i).eta())*(loosedubel2eta - genparticles->at(i).eta()) + (loosedubel2phi - genparticles->at(i).phi())*(loosedubel2phi - genparticles->at(i).phi()));
		double dptoverpt = fabs((loosedubel2pt - genparticles->at(i).pt()) / loosedubel2pt);
		//double amount = dR + dptoverpt;
		double amount = dptoverpt;
		
		//if (amount<minamount2)
		if (amount<minamount2 && dR<0.5)
		{
			minamount2 = amount;
			subleadinggenindex = i;
		}
	}

	if (debug) cout << "hey11" << endl;
	
/////////////////////// Muons //////////////////////////

// 	for (int i=0; i<genpartsize; i++)
// 	{		
// 		if (!tightmupass) continue;
// 		//if (!((genparticles->at(i).status()>20)&&(genparticles->at(i).status()<30))) continue; // if you include 0-10, much more electrons pass this whole thing. but, 20-30 is supposed to be hard scatter process...
// 		if (!((genparticles->at(i).status()>0)&&(genparticles->at(i).status()<10))) continue; // after looking at print statements, determined that 20-30 range is copy of some particles in 1-9 range
// 		//cout << "hey1" << endl;
// 		if (abs(genparticles->at(i).pdgId())!=13) continue;
// 		//cout << "hey2" << endl;
// 		//if (fabs(genparticles->at(i).eta())>2.5) continue;
// 		//cout << "hey3" << endl;
// 		
// 		gen_ele_count++; // trying something
// 		
// 		double dR = sqrt((muonsTLVtight[0].Eta() - genparticles->at(i).eta())*(muonsTLVtight[0].Eta() - genparticles->at(i).eta()) + (muonsTLVtight[0].Phi() - genparticles->at(i).phi())*(muonsTLVtight[0].Phi() - genparticles->at(i).phi()));
// 		double dptoverpt = fabs((muonsTLVtight[0].Pt() - genparticles->at(i).pt()) / muonsTLVtight[0].Pt());
// 		//double amount = dR + dptoverpt;
// 		double amount = dptoverpt;
// 		
// 		//if (amount<minamount)
// 		if (amount<minamount && dR<0.5)
// 		{
// 			minamount = amount;
// 			leadinggenindex = i;
// 		}
// 		
// 	}
// 	
// 	double minamount2 = 99999.;
// 	
// 	for (int i=0; i<genpartsize; i++)
// 	{		
// 		if (!loosedubmupass) continue;
// 		if (!((genparticles->at(i).status()>0)&&(genparticles->at(i).status()<10))) continue;
// 		if (i==leadinggenindex) continue;
// 		if (abs(genparticles->at(i).pdgId())!=13) continue;
// 
// 		double dR = sqrt((loosedubmu2eta - genparticles->at(i).eta())*(loosedubmu2eta - genparticles->at(i).eta()) + (loosedubmu2phi - genparticles->at(i).phi())*(loosedubmu2phi - genparticles->at(i).phi()));
// 		double dptoverpt = fabs((loosedubmu2pt - genparticles->at(i).pt()) / loosedubmu2pt);
// 		//double amount = dR + dptoverpt;
// 		double amount = dptoverpt;
// 		
// 		//if (amount<minamount2)
// 		if (amount<minamount2 && dR<0.5)
// 		{
// 			minamount2 = amount;
// 			subleadinggenindex = i;
// 		}
// 	}	





	if (tightmupass&&loosedubmupass)
	{
		if (debug) cout << "hey11.00" << endl;
		
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[0]))||triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) doublemucount += wgt; // doublemucount++;
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[1]))||triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) doublemucount2 += wgt; // doublemucount2++;
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[2]))||triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) doublemucount3 += wgt; // doublemucount3++;
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[3]))||triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) doublemucount4 += wgt; // doublemucount4++;
		
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[1]))||triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[2]))) doublemucount5 += wgt;

		for (int i=0; i<triggersize; i++) if (triggerResults->accept(i)) allcount_mumu[i] += wgt;

		cout << "wgt: " << wgt << endl;

		numpassedmumucuts += wgt; //numpassedmumucuts++;
		
		mumu_lep1_denom->Fill(muonsTLVtight[0].Pt());
		mumu_lep2_denom->Fill(loosedubmu2pt);
		
		if (debug) cout << "hey11.01" << endl;
		
		if ((leadinggenindex>-1) && genpartsize)
		{

			leadingMuon_pat_gen_dpt->Fill((muonsTLVtight[0].Pt() - genparticles->at(leadinggenindex).pt()) / muonsTLVtight[0].Pt());
			leadingMuon_pat_gen_deta_dphi->Fill( muonsTLVtight[0].Eta() - genparticles->at(leadinggenindex).eta(), muonsTLVtight[0].Phi() - genparticles->at(leadinggenindex).phi());
			if (debug) cout << "hey11.02" << endl;
		}
		
		if (debug) cout << "hey11.02b" << endl;
		
		if ((subleadinggenindex>-1) && genpartsize) 
		{

			subleadingMuon_pat_gen_dpt->Fill((loosedubmu2pt - genparticles->at(subleadinggenindex).pt()) / loosedubmu2pt);
			subleadingMuon_pat_gen_deta_dphi->Fill( loosedubmu2eta - genparticles->at(subleadinggenindex).eta(), loosedubmu2phi - genparticles->at(subleadinggenindex).phi());
			if (debug) cout << "hey11.03" << endl;
		}
		if (debug) cout << "hey11.03b" << endl;
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[0]))) 
		{ 
			lep1_pt_didpass_double_lep[0]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didpass_double_lep[0]->Fill(loosedubmu2pt);
			lep1_eta_didpass_double_lep[0]->Fill(muonsTLVtight[0].Eta()); 
			lep2_eta_didpass_double_lep[0]->Fill(loosedubmu2eta);
			lep1_phi_didpass_double_lep[0]->Fill(muonsTLVtight[0].Phi()); 
			lep2_phi_didpass_double_lep[0]->Fill(loosedubmu2phi);
			lep1_iso_didpass_double_lep[0]->Fill(muonIsosTight[0]); 
			lep2_iso_didpass_double_lep[0]->Fill(loosedubmu2iso);
			if (debug) cout << "hey11.04" << endl;
			 
		}
		if (debug) cout << "hey11.04b" << endl;
		if ((!triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[0])))&&triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) 
		{ 
			lep1_pt_didntpass_double_lep_but_passed_single_lep[0]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didntpass_double_lep_but_passed_single_lep[0]->Fill(loosedubmu2pt);
			lep1_eta_didntpass_double_lep_but_passed_single_lep[0]->Fill(muonsTLVtight[0].Eta());
			lep2_eta_didntpass_double_lep_but_passed_single_lep[0]->Fill(loosedubmu2eta);
			lep1_phi_didntpass_double_lep_but_passed_single_lep[0]->Fill(muonsTLVtight[0].Phi());
			lep2_phi_didntpass_double_lep_but_passed_single_lep[0]->Fill(loosedubmu2phi);
			lep1_iso_didntpass_double_lep_but_passed_single_lep[0]->Fill(muonIsosTight[0]); 
			lep2_iso_didntpass_double_lep_but_passed_single_lep[0]->Fill(loosedubmu2iso);
			if (debug) cout << "hey11.05" << endl;

		}
		if (debug) cout << "hey11.05b" << endl;
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[1]))) 
		{ 
			lep1_pt_didpass_double_lep[1]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didpass_double_lep[1]->Fill(loosedubmu2pt);
			lep1_eta_didpass_double_lep[1]->Fill(muonsTLVtight[0].Eta()); 
			lep2_eta_didpass_double_lep[1]->Fill(loosedubmu2eta);
			lep1_phi_didpass_double_lep[1]->Fill(muonsTLVtight[0].Phi()); 
			lep2_phi_didpass_double_lep[1]->Fill(loosedubmu2phi);
			lep1_iso_didpass_double_lep[1]->Fill(muonIsosTight[0]); 
			lep2_iso_didpass_double_lep[1]->Fill(loosedubmu2iso);			
			 if (debug) cout << "hey11.06" << endl;
		}
		if (debug) cout << "hey11.06b" << endl;
		if ((!triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[1])))&&triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) 
		{ 
			lep1_pt_didntpass_double_lep_but_passed_single_lep[1]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didntpass_double_lep_but_passed_single_lep[1]->Fill(loosedubmu2pt);
			lep1_eta_didntpass_double_lep_but_passed_single_lep[1]->Fill(muonsTLVtight[0].Eta());
			lep2_eta_didntpass_double_lep_but_passed_single_lep[1]->Fill(loosedubmu2eta);
			lep1_phi_didntpass_double_lep_but_passed_single_lep[1]->Fill(muonsTLVtight[0].Phi());
			lep2_phi_didntpass_double_lep_but_passed_single_lep[1]->Fill(loosedubmu2phi);
			lep1_iso_didntpass_double_lep_but_passed_single_lep[1]->Fill(muonIsosTight[0]); 
			lep2_iso_didntpass_double_lep_but_passed_single_lep[1]->Fill(loosedubmu2iso);
			if (debug) cout << "hey11.07" << endl;				 
		}
		if (debug) cout << "hey11.07b" << endl;
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[2]))) 
		{ 
			lep1_pt_didpass_double_lep[2]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didpass_double_lep[2]->Fill(loosedubmu2pt);
			lep1_eta_didpass_double_lep[2]->Fill(muonsTLVtight[0].Eta()); 
			lep2_eta_didpass_double_lep[2]->Fill(loosedubmu2eta);
			lep1_phi_didpass_double_lep[2]->Fill(muonsTLVtight[0].Phi()); 
			lep2_phi_didpass_double_lep[2]->Fill(loosedubmu2phi);
			lep1_iso_didpass_double_lep[2]->Fill(muonIsosTight[0]); 
			lep2_iso_didpass_double_lep[2]->Fill(loosedubmu2iso);
			if (debug) cout << "hey11.08" << endl;
			 
		}
		if (debug) cout << "hey11.08b" << endl;
		if ((!triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[2])))&&triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) 
		{ 
			lep1_pt_didntpass_double_lep_but_passed_single_lep[2]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didntpass_double_lep_but_passed_single_lep[2]->Fill(loosedubmu2pt);
			lep1_eta_didntpass_double_lep_but_passed_single_lep[2]->Fill(muonsTLVtight[0].Eta());
			lep2_eta_didntpass_double_lep_but_passed_single_lep[2]->Fill(loosedubmu2eta);
			lep1_phi_didntpass_double_lep_but_passed_single_lep[2]->Fill(muonsTLVtight[0].Phi());
			lep2_phi_didntpass_double_lep_but_passed_single_lep[2]->Fill(loosedubmu2phi);
			lep1_iso_didntpass_double_lep_but_passed_single_lep[2]->Fill(muonIsosTight[0]); 
			lep2_iso_didntpass_double_lep_but_passed_single_lep[2]->Fill(loosedubmu2iso);
			if (debug) cout << "hey11.09" << endl;
		}
		if (debug) cout << "hey11.09b" << endl;
		if (triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[3]))) 
		{ 
			lep1_pt_didpass_double_lep[3]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didpass_double_lep[3]->Fill(loosedubmu2pt);
			lep1_eta_didpass_double_lep[3]->Fill(muonsTLVtight[0].Eta()); 
			lep2_eta_didpass_double_lep[3]->Fill(loosedubmu2eta);
			lep1_phi_didpass_double_lep[3]->Fill(muonsTLVtight[0].Phi()); 
			lep2_phi_didpass_double_lep[3]->Fill(loosedubmu2phi);
			lep1_iso_didpass_double_lep[3]->Fill(muonIsosTight[0]); 
			lep2_iso_didpass_double_lep[3]->Fill(loosedubmu2iso);			
			if (debug) cout << "hey11.10" << endl;
		}
		if (debug) cout << "hey11.10b" << endl;
		if ((!triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[3])))&&triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) 
		{ 
			lep1_pt_didntpass_double_lep_but_passed_single_lep[3]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didntpass_double_lep_but_passed_single_lep[3]->Fill(loosedubmu2pt);
			lep1_eta_didntpass_double_lep_but_passed_single_lep[3]->Fill(muonsTLVtight[0].Eta());
			lep2_eta_didntpass_double_lep_but_passed_single_lep[3]->Fill(loosedubmu2eta);
			lep1_phi_didntpass_double_lep_but_passed_single_lep[3]->Fill(muonsTLVtight[0].Phi());
			lep2_phi_didntpass_double_lep_but_passed_single_lep[3]->Fill(loosedubmu2phi);
			lep1_iso_didntpass_double_lep_but_passed_single_lep[3]->Fill(muonIsosTight[0]); 
			lep2_iso_didntpass_double_lep_but_passed_single_lep[3]->Fill(loosedubmu2iso);
			if (debug) cout << "hey11.11" << endl;
		}
		if (debug) cout << "hey11.11b" << endl;
		// gen stuff
		if ((!triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[2])))&&(!triggerResults->accept(hltConfig_.triggerIndex(extra_trig))))
		{			
			
			if (leadinggenindex==-1 && subleadinggenindex==-1) gen_mu_info_vs_hlt->Fill(0.,0.,wgt);
			if ((leadinggenindex>=-1 && subleadinggenindex==-1) || (leadinggenindex==-1 && subleadinggenindex>=-1)) gen_mu_info_vs_hlt->Fill(0.,1.,wgt);
			if (leadinggenindex>=-1 && subleadinggenindex>=-1) gen_mu_info_vs_hlt->Fill(0.,2.,wgt);
			if (debug) cout << "hey11.12" << endl;
			
		}
		if (debug) cout << "hey11.12b" << endl;
		if ((!triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[2])))&&(triggerResults->accept(hltConfig_.triggerIndex(extra_trig))))
		{
			if (leadinggenindex==-1 && subleadinggenindex==-1) gen_mu_info_vs_hlt->Fill(1.,0.,wgt);
			if ((leadinggenindex>=-1 && subleadinggenindex==-1) || (leadinggenindex==-1 && subleadinggenindex>=-1)) gen_mu_info_vs_hlt->Fill(1.,1.,wgt);
			if (leadinggenindex>=-1 && subleadinggenindex>=-1) gen_mu_info_vs_hlt->Fill(1.,2.,wgt);
			if (debug) cout << "hey11.13" << endl;
		}
		if (debug) cout << "hey11.13b" << endl;
		if ((triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[2])))&&(!triggerResults->accept(hltConfig_.triggerIndex(extra_trig))))
		{
			if (leadinggenindex==-1 && subleadinggenindex==-1) gen_mu_info_vs_hlt->Fill(2.,0.,wgt);
			if ((leadinggenindex>=-1 && subleadinggenindex==-1) || (leadinggenindex==-1 && subleadinggenindex>=-1)) gen_mu_info_vs_hlt->Fill(2.,1.,wgt);
			if (leadinggenindex>=-1 && subleadinggenindex>=-1) gen_mu_info_vs_hlt->Fill(2.,2.,wgt);
			if (debug) cout << "hey11.14" << endl;
		}
		if (debug) cout << "hey11.14b" << endl;
		if ((triggerResults->accept(hltConfig_.triggerIndex(mumutriggerstostudy[2])))&&(triggerResults->accept(hltConfig_.triggerIndex(extra_trig))))
		{
			
			if (leadinggenindex==-1 && subleadinggenindex==-1) gen_mu_info_vs_hlt->Fill(3.,0.,wgt);
			if ((leadinggenindex>=-1 && subleadinggenindex==-1) || (leadinggenindex==-1 && subleadinggenindex>=-1)) gen_mu_info_vs_hlt->Fill(3.,1.,wgt);
			if (leadinggenindex>=-1 && subleadinggenindex>=-1) gen_mu_info_vs_hlt->Fill(3.,2.,wgt);
			if (debug) cout << "hey11.15" << endl;
		}
		
		if (debug) cout << "hey11.1" << endl;
	}

	if (tightelepass&&loosedubelepass)
	{
		if (triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[0]))||triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) doubleelecount += wgt; //doubleelecount++;
		if (triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[1]))||triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) doubleelecount2 += wgt; //doubleelecount2++;
		numpassedelelcuts += wgt; //numpassedelelcuts++;
		
		for (int i=0; i<triggersize; i++) if (triggerResults->accept(i)) allcount_elel[i] += wgt;
		
		elel_lep1_denom->Fill(elesTLVtight[0].Pt());
		elel_lep2_denom->Fill(loosedubel2pt);
		
		// cout << "  " << endl;
// 		cout << "  " << endl;
// 		cout << "tight ele kine: " << elesTLVtight[0].Pt() << "  " << elesTLVtight[0].Eta() << "  " << elesTLVtight[0].Phi() << endl;
// 		cout << "loose ele kine: " << loosedubel2pt << "  " << loosedubel2eta << "  " << loosedubel2phi << endl;
// 		cout << "gen eles: " << endl;
// 		for (int i=0; i<genpartsize; i++)
// 		{
// 			if (abs(genparticles->at(i).pdgId())!=11) continue;
// 			cout << "pdgId: " << genparticles->at(i).pdgId() << "status: " << genparticles->at(i).status() << "kine: " << genparticles->at(i).pt() << "  " << genparticles->at(i).eta() << "  " << genparticles->at(i).phi() << endl;
// 		}
		
		if ((leadinggenindex>-1) && genpartsize)
		{
			leadingEle_pat_gen_chosen_reso->Fill(minamount);
			leadingEle_pat_gen_dpt->Fill((elesTLVtight[0].Pt() - genparticles->at(leadinggenindex).pt()) / elesTLVtight[0].Pt());
			leadingEle_pat_gen_deta_dphi->Fill( elesTLVtight[0].Eta() - genparticles->at(leadinggenindex).eta(), elesTLVtight[0].Phi() - genparticles->at(leadinggenindex).phi());
		}
			
		if ((subleadinggenindex>-1) && genpartsize) 
		{
			subleadingEle_pat_gen_chosen_reso->Fill(minamount2);
			subleadingEle_pat_gen_dpt->Fill((loosedubel2pt - genparticles->at(subleadinggenindex).pt()) / loosedubel2pt);
			subleadingEle_pat_gen_deta_dphi->Fill( loosedubel2eta - genparticles->at(subleadinggenindex).eta(), loosedubel2phi - genparticles->at(subleadinggenindex).phi());
		}
		
		
		double leadingpt = -99.;
		double subleadingpt = -99.;
		double leadingeta = -99.;
		double subleadingeta = -99.;
		
		if (elesTLVtight[0].Pt() > loosedubel2pt)
		{
			leadingpt = elesTLVtight[0].Pt(); 
			subleadingpt = loosedubel2pt;
			leadingeta = elesTLVtight[0].Eta(); 
			subleadingeta = loosedubel2eta;
		}
		if (elesTLVtight[0].Pt() <= loosedubel2pt)
		{
			leadingpt = loosedubel2pt;
			subleadingpt = elesTLVtight[0].Pt(); 
			leadingeta = loosedubel2eta;
			subleadingeta = elesTLVtight[0].Eta();
		}
		
		if (triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[0]))) 
		{ 
			lep1_pt_didpass_double_lep[4]->Fill(elesTLVtight[0].Pt()); 
			lep2_pt_didpass_double_lep[4]->Fill(loosedubel2pt);
			lep1_eta_didpass_double_lep[4]->Fill(elesTLVtight[0].Eta()); 
			lep2_eta_didpass_double_lep[4]->Fill(loosedubel2eta);
			lep1_phi_didpass_double_lep[4]->Fill(elesTLVtight[0].Phi()); 
			lep2_phi_didpass_double_lep[4]->Fill(loosedubel2phi);
			lep1_iso_didpass_double_lep[4]->Fill(electronIsosTight[0]); 
			lep2_iso_didpass_double_lep[4]->Fill(loosedubel2iso);	
			
			if (objs_found) lep1_pt_didpass_double_lep_PATobj_x_HLTobj_y[4]->Fill(leadingpt,last_module_with_saved_tags_obj_pt[0]);
			if (objs_found>1) lep2_pt_didpass_double_lep_PATobj_x_HLTobj_y[4]->Fill(subleadingpt,last_module_with_saved_tags_obj_pt[1]);
			if (objs_found) lep1_eta_didpass_double_lep_PATobj_x_HLTobj_y[4]->Fill(leadingeta,last_module_with_saved_tags_obj_eta[0]);
			if (objs_found>1) lep2_eta_didpass_double_lep_PATobj_x_HLTobj_y[4]->Fill(subleadingeta,last_module_with_saved_tags_obj_eta[1]);
			 
		}
		
		if ((!triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[0])))&&triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) 
		{ 
			lep1_pt_didntpass_double_lep_but_passed_single_lep[4]->Fill(elesTLVtight[0].Pt()); 
			lep2_pt_didntpass_double_lep_but_passed_single_lep[4]->Fill(loosedubel2pt);
			lep1_eta_didntpass_double_lep_but_passed_single_lep[4]->Fill(elesTLVtight[0].Eta());
			lep2_eta_didntpass_double_lep_but_passed_single_lep[4]->Fill(loosedubel2eta);
			lep1_phi_didntpass_double_lep_but_passed_single_lep[4]->Fill(elesTLVtight[0].Phi());
			lep2_phi_didntpass_double_lep_but_passed_single_lep[4]->Fill(loosedubel2phi);
			lep1_iso_didntpass_double_lep_but_passed_single_lep[4]->Fill(electronIsosTight[0]); 
			lep2_iso_didntpass_double_lep_but_passed_single_lep[4]->Fill(loosedubel2iso);  		// 107
			
			
			if (objs_found) lep1_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[4]->Fill(leadingpt,last_module_with_saved_tags_obj_pt[0]);
			if (objs_found>1) lep2_pt_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[4]->Fill(subleadingpt,last_module_with_saved_tags_obj_pt[1]);
			if (objs_found) lep1_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[4]->Fill(leadingeta,last_module_with_saved_tags_obj_eta[0]);	// 93
			if (objs_found>1) lep2_eta_didntpass_double_lep_but_passed_single_lep_PATobj_x_HLTobj_y[4]->Fill(subleadingeta,last_module_with_saved_tags_obj_eta[1]);	// 93		
				
			//if (objs_found) summaryTree->Fill();	// 
					 
		}
		
		if (triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[1]))) 
		{ 
			lep1_pt_didpass_double_lep[5]->Fill(elesTLVtight[0].Pt()); 
			lep2_pt_didpass_double_lep[5]->Fill(loosedubel2pt);
			lep1_eta_didpass_double_lep[5]->Fill(elesTLVtight[0].Eta()); 
			lep2_eta_didpass_double_lep[5]->Fill(loosedubel2eta);
			lep1_phi_didpass_double_lep[5]->Fill(elesTLVtight[0].Phi()); 
			lep2_phi_didpass_double_lep[5]->Fill(loosedubel2phi);
			lep1_iso_didpass_double_lep[5]->Fill(electronIsosTight[0]); 
			lep2_iso_didpass_double_lep[5]->Fill(loosedubel2iso);
						
			 
		}
		if ((!triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[1])))&&triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) 
		{ 
			lep1_pt_didntpass_double_lep_but_passed_single_lep[5]->Fill(elesTLVtight[0].Pt()); 
			lep2_pt_didntpass_double_lep_but_passed_single_lep[5]->Fill(loosedubel2pt);
			lep1_eta_didntpass_double_lep_but_passed_single_lep[5]->Fill(elesTLVtight[0].Eta());
			lep2_eta_didntpass_double_lep_but_passed_single_lep[5]->Fill(loosedubel2eta);
			lep1_phi_didntpass_double_lep_but_passed_single_lep[5]->Fill(elesTLVtight[0].Phi());
			lep2_phi_didntpass_double_lep_but_passed_single_lep[5]->Fill(loosedubel2phi);
			lep1_iso_didntpass_double_lep_but_passed_single_lep[5]->Fill(electronIsosTight[0]); 
			lep2_iso_didntpass_double_lep_but_passed_single_lep[5]->Fill(loosedubel2iso);						
			 
		}
		
		
		
		// gen stuff
		if ((!triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[0])))&&(!triggerResults->accept(hltConfig_.triggerIndex(extra_trig))))
		{
			//if (gen_ele_count==0) gen_ele_info_vs_hlt->Fill(0.,0.);
			//if (gen_ele_count==1) gen_ele_info_vs_hlt->Fill(0.,1.);
			//if (gen_ele_count>=2) gen_ele_info_vs_hlt->Fill(0.,2.);			
			
			if (leadinggenindex==-1 && subleadinggenindex==-1) gen_ele_info_vs_hlt->Fill(0.,0.,wgt);
			if ((leadinggenindex>=-1 && subleadinggenindex==-1) || (leadinggenindex==-1 && subleadinggenindex>=-1)) gen_ele_info_vs_hlt->Fill(0.,1.,wgt);
			if (leadinggenindex>=-1 && subleadinggenindex>=-1) gen_ele_info_vs_hlt->Fill(0.,2.,wgt);
			
			
		}
		if ((!triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[0])))&&(triggerResults->accept(hltConfig_.triggerIndex(extra_trig))))
		{
			if (leadinggenindex==-1 && subleadinggenindex==-1) gen_ele_info_vs_hlt->Fill(1.,0.,wgt);
			if ((leadinggenindex>=-1 && subleadinggenindex==-1) || (leadinggenindex==-1 && subleadinggenindex>=-1)) gen_ele_info_vs_hlt->Fill(1.,1.,wgt);
			if (leadinggenindex>=-1 && subleadinggenindex>=-1) gen_ele_info_vs_hlt->Fill(1.,2.,wgt);
			
		}
		if ((triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[0])))&&(!triggerResults->accept(hltConfig_.triggerIndex(extra_trig))))
		{
			if (leadinggenindex==-1 && subleadinggenindex==-1) gen_ele_info_vs_hlt->Fill(2.,0.,wgt);
			if ((leadinggenindex>=-1 && subleadinggenindex==-1) || (leadinggenindex==-1 && subleadinggenindex>=-1)) gen_ele_info_vs_hlt->Fill(2.,1.,wgt);
			if (leadinggenindex>=-1 && subleadinggenindex>=-1) gen_ele_info_vs_hlt->Fill(2.,2.,wgt);
			
		}
		if ((triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[0])))&&(triggerResults->accept(hltConfig_.triggerIndex(extra_trig))))
		{
			
			if (leadinggenindex==-1 && subleadinggenindex==-1) gen_ele_info_vs_hlt->Fill(3.,0.,wgt);
			if ((leadinggenindex>=-1 && subleadinggenindex==-1) || (leadinggenindex==-1 && subleadinggenindex>=-1)) gen_ele_info_vs_hlt->Fill(3.,1.,wgt);
			if (leadinggenindex>=-1 && subleadinggenindex>=-1) gen_ele_info_vs_hlt->Fill(3.,2.,wgt);
		}

		if (debug) cout << "hey11.2" << endl;
	
	}

	if (tightmupass&&muele_looseelepass)
	{
		if (triggerResults->accept(hltConfig_.triggerIndex(mueltriggerstostudy[0]))||triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) muelecount += wgt; //muelecount++;
		numpassedmuelcuts += wgt; //numpassedmuelcuts++;
		
		for (int i=0; i<triggersize; i++) if (triggerResults->accept(i)) allcount_muel[i] += wgt;
		
		muel_lep1_denom->Fill(muonsTLVtight[0].Pt());
		muel_lep2_denom->Fill(elesTLVloose[0].Pt());
		
		if (triggerResults->accept(hltConfig_.triggerIndex(mueltriggerstostudy[0]))) 
		{ 
			lep1_pt_didpass_double_lep[6]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didpass_double_lep[6]->Fill(elesTLVloose[0].Pt());
			lep1_eta_didpass_double_lep[6]->Fill(muonsTLVtight[0].Eta());
			lep2_eta_didpass_double_lep[6]->Fill(elesTLVloose[0].Eta());
			lep1_phi_didpass_double_lep[6]->Fill(muonsTLVtight[0].Phi());
			lep2_phi_didpass_double_lep[6]->Fill(elesTLVloose[0].Phi());
			lep1_iso_didpass_double_lep[6]->Fill(muonIsosTight[0]);
			lep2_iso_didpass_double_lep[6]->Fill(electronIsosLoose[0]);
			
			 
		}
		if ((!triggerResults->accept(hltConfig_.triggerIndex(mueltriggerstostudy[0])))&&triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) 
		{ 
			lep1_pt_didntpass_double_lep_but_passed_single_lep[6]->Fill(muonsTLVtight[0].Pt()); 
			lep2_pt_didntpass_double_lep_but_passed_single_lep[6]->Fill(elesTLVloose[0].Pt());
			lep1_eta_didntpass_double_lep_but_passed_single_lep[6]->Fill(muonsTLVtight[0].Eta());
			lep2_eta_didntpass_double_lep_but_passed_single_lep[6]->Fill(elesTLVloose[0].Eta());
			lep1_phi_didntpass_double_lep_but_passed_single_lep[6]->Fill(muonsTLVtight[0].Phi());
			lep2_phi_didntpass_double_lep_but_passed_single_lep[6]->Fill(elesTLVloose[0].Phi());
			lep1_iso_didntpass_double_lep_but_passed_single_lep[6]->Fill(muonIsosTight[0]);
			lep2_iso_didntpass_double_lep_but_passed_single_lep[6]->Fill(electronIsosLoose[0]);
						 
		}	
		if (debug) cout << "hey11.3" << endl;
	}

	if (tightelepass&&muele_loosemupass)
	{
		if (triggerResults->accept(hltConfig_.triggerIndex(elmutriggerstostudy[0]))||triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) elemucount += wgt; //elemucount++;
		numpassedelmucuts += wgt; //numpassedelmucuts++;
		
		for (int i=0; i<triggersize; i++) if (triggerResults->accept(i)) allcount_elmu[i] += wgt;
		
		elmu_lep1_denom->Fill(elesTLVtight[0].Pt());
		elmu_lep2_denom->Fill(muonsTLVloose[0].Pt());
		
		if (triggerResults->accept(hltConfig_.triggerIndex(elmutriggerstostudy[0]))) 
		{ 
			lep1_pt_didpass_double_lep[7]->Fill(elesTLVtight[0].Pt()); 
			lep2_pt_didpass_double_lep[7]->Fill(muonsTLVloose[0].Pt());
			lep1_eta_didpass_double_lep[7]->Fill(elesTLVtight[0].Eta());
			lep2_eta_didpass_double_lep[7]->Fill(muonsTLVloose[0].Eta());
			lep1_phi_didpass_double_lep[7]->Fill(elesTLVtight[0].Phi());
			lep2_phi_didpass_double_lep[7]->Fill(muonsTLVloose[0].Phi());
			lep1_iso_didpass_double_lep[7]->Fill(electronIsosTight[0]);
			lep2_iso_didpass_double_lep[7]->Fill(muonIsosLoose[0]);			
			 
		}
		if ((!triggerResults->accept(hltConfig_.triggerIndex(elmutriggerstostudy[0])))&&triggerResults->accept(hltConfig_.triggerIndex(extra_trig))) 
		{ 
			lep1_pt_didntpass_double_lep_but_passed_single_lep[7]->Fill(elesTLVtight[0].Pt()); 
			lep2_pt_didntpass_double_lep_but_passed_single_lep[7]->Fill(muonsTLVloose[0].Pt());
			lep1_eta_didntpass_double_lep_but_passed_single_lep[7]->Fill(elesTLVtight[0].Eta());
			lep2_eta_didntpass_double_lep_but_passed_single_lep[7]->Fill(muonsTLVloose[0].Eta());
			lep1_phi_didntpass_double_lep_but_passed_single_lep[7]->Fill(elesTLVtight[0].Phi());
			lep2_phi_didntpass_double_lep_but_passed_single_lep[7]->Fill(muonsTLVloose[0].Phi());
			lep1_iso_didntpass_double_lep_but_passed_single_lep[7]->Fill(electronIsosTight[0]);
			lep2_iso_didntpass_double_lep_but_passed_single_lep[7]->Fill(muonIsosLoose[0]);			
			 
		}	
		if (debug) cout << "hey11.4" << endl;
		
	}

	
	
// 	edm::Handle<pat::TriggerObjectStandAloneCollection> eleHLTMatcheshand;
// 	event.getByLabel("selectedPatTrigger",eleHLTMatcheshand);
// 	pat::TriggerObjectStandAloneCollection eleHLTMatches = *eleHLTMatcheshand;
// 	//pat::TriggerObjectStandAloneCollection eleHLTMatches = eleHLTMatchesallpaths.pathindices[hltConfig_.triggerIndex(eleltriggerstostudy[0])];
// 	
// 	//eleHLTMatches.unpackPathNames( hltConfig_.triggerNames );
// 	//const edm::TriggerNames triggerNames = event.triggerNames(*triggerResults);
// 	
// 	
// 	//trigRes theTrigRes = GetTriggers(event);
// 	///const edm::TriggerNames triggerNames = event.triggerNames(*theTrigRes);
// 	//eleHLTMatches.unpackPathNames( triggerNames );
// 	
// 	
// 	//pat::
// 	//TriggerEvent::
// 	//TriggerObjectRefVector pathObjects( const std::string & namePath, bool firing = true ) const;
// 	
// 	if (selectedElectrons_tight.size())
// 	{
// 	
// 		//const pat::TriggerObjectStandAloneCollection eleHLTMatches = eleHLTMatchesallpaths.triggerObjectMatchesByPath( eleltriggerstostudy[0]); // ,false,false 
// 	
// 	
// 		int trigobjsize = eleHLTMatches.size();
// 	
// 		cout << "trigobjsize " << trigobjsize << "path was run: " << triggerResults->wasrun(hltConfig_.triggerIndex(eleltriggerstostudy[0])) << "path was accept: " << triggerResults->accept(hltConfig_.triggerIndex(eleltriggerstostudy[0])) << endl;
// 	
// 		//if (trigobjsize) 
// 		for (int i=0; i<trigobjsize; i++)
// 		//for (
// 		{			
// 			//if (eleHLTMatches[i].path(eleltriggerstostudy[0])) {
// 			//if (eleHLTMatches[i].pathIndices[0]==10) {
// 			
// 			vector<int> ridiculous = eleHLTMatches[i].return_pathIndices();
// 			
// 			bool foundit = false;
// 			
// 			for (int j=0; j<(int)ridiculous.size(); j++)
// 			{
// 				if ((int)ridiculous[j]==(int)hltConfig_.triggerIndex(eleltriggerstostudy[0])) foundit = true;
// 			}
// 			
// 			
// 			//if (!ridiculous.size()) break;
// 			
// 			//bool continueit = false;
// 			
// 			//for (int j=0; j<ridiculous.size(); j++)
// 			//{
// 				//bool thepathishere = false;
// 				if (foundit) {
// 			cout << ridiculous.size() << endl;
// 			cout << eleHLTMatches[i].pdgId() << endl;
// 			cout << eleHLTMatches[i].pt() << endl;
// 			cout << eleHLTMatches[i].eta() << endl;
// 			cout << eleHLTMatches[i].phi() << endl;
// 			cout << " " << endl; }
// 		}
// 	
// 	}
// 	
// 	
	if (debug) cout << "hey12" << endl;
		
////////////////////////////////////////////////////////////////////////////////////

	
	
	// variables->numLooseMuons = numLooseMuons;
// 	variables->numLooseElectrons = numLooseElectrons;	
// 	variables->numTightMuons = numTightMuons;
// 	variables->numTightElectrons = numTightElectrons;
	
// 	// assign values to tree variables:
// 	numLooseMuons_intree = numLooseMuons;
// 	numLooseElectrons_intree = numLooseElectrons;
// 	numTightMuons_intree = numTightMuons;
// 	numTightElectrons_intree = numTightElectrons;
// 	
// 	//testvect.push_back(3.);
// 	//testvect.push_back(11.);
// 	
// 	//testTLV_intree = theMET;
// 	
// 	mcwgt_intree = wgt;
// 	
// 	jetpartonflavor_intree = vecJetPdgIDs;
// 	
// 	Jets_intree = jetsTLVloose;
// 	MET_intree = theMET;
// 	LooseElectrons_intree = elesTLVloose;
// 	LooseMuons_intree = muonsTLVloose;
// 	TightElectrons_intree = elesTLVtight;
// 	TightMuons_intree = muonsTLVtight;
// 	JetCSV_intree = ReturnBTagDisc(selectedJets_loose_noSys_unsorted);
// 		
// 	
// 	
// 	// fill it:
 	summaryTree->Fill();
	
	if (debug) cout << "hey13" << endl;
	
	
} // end for each event

void TriggerAna::beginRun(edm::Run const& run, edm::EventSetup const& evsetup){
		
	// This has to be done here (not at beginjob):
	
	bool changed = true;
	if (hltConfig_.init(run, evsetup, hltTag, changed)) std::cout << "HLT config with process name " << hltTag << " successfully extracted" << std::endl;
	
	else std::cout << "Warning, didn't find process " << hltTag << std::endl;
	
	std::cout << " HLTConfig processName " << hltConfig_.processName() << " tableName " << hltConfig_.tableName() << " size " << hltConfig_.size() << std::endl; // " globalTag: " << hltConfig_.globalTag() << std::endl;
	
}
void TriggerAna::endRun(edm::Run const& run, edm::EventSetup const& evsetup){

	//cout << doublmucount << endl;
	//cout << numpassedcuts << endl;
	cout << "total events processed: " << eventcount << endl;
	
	cout << "trigger results" << endl;
	
// 	cout << mumutriggerstostudy[0] << "  " << doublemucount << "out of " << numpassedmumucuts << endl;
// 	cout << mumutriggerstostudy[1] << "  " << doublemucount2 << "out of " << numpassedmumucuts << endl;
// 	cout << mumutriggerstostudy[2] << "  " << doublemucount3 << "out of " << numpassedmumucuts << endl;
// 	cout << mumutriggerstostudy[3] << "  " << doublemucount4 << "out of " << numpassedmumucuts << endl;
// 	cout << "  " << endl;
// 	cout << eleltriggerstostudy[0] << "  " << doubleelecount << "out of " << numpassedelelcuts << endl;
// 	cout << eleltriggerstostudy[1] << "  " << doubleelecount2 << "out of " << numpassedelelcuts << endl;
// 	cout << "  " << endl;
// 	cout << mueltriggerstostudy[0] << "  " << muelecount << "out of " << numpassedmuelcuts << endl;
// 	cout << "  " << endl;
// 	cout << elmutriggerstostudy[0] << "  " << elemucount << "out of " << numpassedelmucuts << endl;

	cout << "  " << endl;
	cout << "triggers or'd with " << extra_trig << ": " << endl;
	cout << mumutriggerstostudy[0] << "  " << doublemucount << "out of " << numpassedmumucuts << ", fraction: " << (double)doublemucount/numpassedmumucuts << endl;
	cout << mumutriggerstostudy[1] << "  " << doublemucount2 << "out of " << numpassedmumucuts << ", fraction: " << (double)doublemucount2/numpassedmumucuts << endl;
	cout << mumutriggerstostudy[2] << "  " << doublemucount3 << "out of " << numpassedmumucuts << ", fraction: " << (double)doublemucount3/numpassedmumucuts << endl;
	cout << mumutriggerstostudy[3] << "  " << doublemucount4 << "out of " << numpassedmumucuts << ", fraction: " << (double)doublemucount4/numpassedmumucuts << endl;
	//cout << mumutriggerstostudy[1] << " or " <<  mumutriggerstostudy[2]  << "  " << doublemucount5 << "out of " << numpassedmumucuts << ", fraction: " << (double)doublemucount5/numpassedmumucuts << endl;
	cout << "  " << endl;
	cout << eleltriggerstostudy[0] << "  " << doubleelecount << "out of " << numpassedelelcuts << ", fraction: " << (double)doubleelecount/numpassedelelcuts << endl;
	cout << eleltriggerstostudy[1] << "  " << doubleelecount2 << "out of " << numpassedelelcuts << ", fraction: " << (double)doubleelecount2/numpassedelelcuts << endl;
	cout << "  " << endl;
	cout << mueltriggerstostudy[0] << "  " << muelecount << "out of " << numpassedmuelcuts << ", fraction: " << (double)muelecount/numpassedmuelcuts << endl;
	cout << "  " << endl;
	cout << elmutriggerstostudy[0] << "  " << elemucount << "out of " << numpassedelmucuts << ", fraction: " << (double)elemucount/numpassedelmucuts << endl;
	cout << "  " << endl;
	cout << doublemucount5 <<  "  "  << numpassedmumucuts << endl;
	cout << "  " << endl;
	cout << "for individual trigger counts, see below (offline selection is given)" << endl;
	cout << "  " << endl;
	cout << "  " << endl;
	
	int numtrigs = hltConfig_.size();
	std::vector<std::string> myTriggernames = hltConfig_.triggerNames();
	cout << "  " << endl;
	cout << "  " << endl;
	cout << "triggers firing on mumu selection: " << endl;
	cout << "  " << endl;
	for (int i =0; i<numtrigs; i++) cout << myTriggernames[i] << ":  " << allcount_mumu[i] << endl;
	cout << "  " << endl;
	cout << "  " << endl;
	cout << "triggers firing on elel selection: " << endl;
	for (int i =0; i<numtrigs; i++) cout << myTriggernames[i] << ":  " << allcount_elel[i] << endl;
	cout << "  " << endl;
	cout << "  " << endl;
	cout << "triggers firing on muel selection: " << endl;
	for (int i =0; i<numtrigs; i++) cout << myTriggernames[i] << ":  " << allcount_muel[i] << endl;
	cout << "  " << endl;
	cout << "  " << endl;
	cout << "triggers firing on elmu selection: " << endl;
	for (int i =0; i<numtrigs; i++) cout << myTriggernames[i] << ":  " << allcount_elmu[i] << endl;
	

	
// 	std::vector<std::string> the_saved_tag_modules = hltConfig_.saveTagsModules(hltConfig_.triggerIndex("HLT_Mu17_Mu8_v1"));
// 	cout << "HLT_Mu17_Mu8_v1" << ": " << the_saved_tag_modules[0] << endl;
// 	std::vector<std::string> the_saved_tag_modules2 = hltConfig_.saveTagsModules(hltConfig_.triggerIndex("HLT_Mu17_TkMu8_v1"));
// 	cout << "HLT_Mu17_TkMu8_v1" << ": " << the_saved_tag_modules2[0] << endl;
// 	std::vector<std::string> the_saved_tag_modules3 = hltConfig_.saveTagsModules(hltConfig_.triggerIndex("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v1"));
// 	cout << "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v1" << ": " << the_saved_tag_modules3[0] << endl;
// 	std::vector<std::string> the_saved_tag_modules4 = hltConfig_.saveTagsModules(hltConfig_.triggerIndex("HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v1"));
// 	cout << "HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v1" << ": " << the_saved_tag_modules4[0] << endl;
// 	std::vector<std::string> the_saved_tag_modules5 = hltConfig_.saveTagsModules(hltConfig_.triggerIndex("HLT_Ele17_Ele8_Gsf_v1"));
// 	cout << "HLT_Ele17_Ele8_Gsf_v1" << ": " << the_saved_tag_modules5[0] << endl;
// 	std::vector<std::string> the_saved_tag_modules6 = hltConfig_.saveTagsModules(hltConfig_.triggerIndex("HLT_Ele23_Ele12_CaloId_TrackId_Iso_v1"));
// 	cout << "HLT_Ele23_Ele12_CaloId_TrackId_Iso_v1" << ": " << the_saved_tag_modules6[0] << endl;
// 	std::vector<std::string> the_saved_tag_modules7 = hltConfig_.saveTagsModules(hltConfig_.triggerIndex("HLT_Mu23_TrkIsoVVL_Ele12_Gsf_CaloId_TrackId_Iso_MediumWP_v1"));
// 	cout << "HLT_Mu23_TrkIsoVVL_Ele12_Gsf_CaloId_TrackId_Iso_MediumWP_v1" << ": " << the_saved_tag_modules7[0] << endl;
// 	std::vector<std::string> the_saved_tag_modules8 = hltConfig_.saveTagsModules(hltConfig_.triggerIndex("HLT_Mu8_TrkIsoVVL_Ele23_Gsf_CaloId_TrackId_Iso_MediumWP_v1"));
// 	cout << "HLT_Mu8_TrkIsoVVL_Ele23_Gsf_CaloId_TrackId_Iso_MediumWP_v1" << ": " << the_saved_tag_modules8[0] << endl;
	
	

} // anything special for the end of a run

void TriggerAna::beginLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& evsetup){} // anything special for the beginning of a lumi block
void TriggerAna::endLuminosityBlock(edm::LuminosityBlock const& lumi, edm::EventSetup const& evsetup){} // anything special for the end of a lumi block


DEFINE_FWK_MODULE(TriggerAna);
