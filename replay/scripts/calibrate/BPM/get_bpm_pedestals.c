#include "TStyle.h"
#include <sstream> 
#include <string>
#include <cmath>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <TSystem.h> 
#include <TGraph.h>
#include <TString.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TFile.h>
#include <TChain.h>
#include <TH1F.h>
#include <TChain.h>
#include <TTree.h>
#include <TF1.h>
#include <TVector.h>
#include <TNtuple.h>
#include <iostream>     //for using cout
#include <stdlib.h>     //for using the function sleep

/*     get_bpm_pedestals() 
Using a bpm bedestal run, this code will produce the pedestal locations of the BPMs for both A and B!
In order to use this code, you must add the location and file name of the pedestal run's root file, also record which detector R or L.

*/

/*Previsously used ped files
left_gmp_12462.root
left_online_13755.root
right_gmp_21787.root
*/


void get_bpm_pedestals(int run=0, char side='i'){
	cout<<endl<<endl;
	
	const char* exp=getenv("EXPERIMENT");
	//enter location of ped run.
	const char* root_dir= "/chafs1/work1/tritium/Rootfiles";//"/adaqfs/home/a-onl/tritium/replay/RootFiles";
	if(run==0){cout << "Please enter the run number of your BPM pedestal run: \n"; 
		cin >> run;}

	if(side=='i'){cout << "Please enter the letter L or R to choose which HRS to use: \n";
		cin >>side;}

	gStyle->SetOptFit(1111);
	TChain * t = new TChain("T");
	t->Add(Form("%s/tritium_online_%d.root",root_dir,run));
		
	int nentries = t->GetEntries();
	if(nentries <= 0){cout <<endl<<endl;exit(1);return; }

	int Run=run; //please enter the run number here for output file

	TVector pedestal(8);

	TCanvas* c1 = new TCanvas("c1","BPM Pedestals");
	TCanvas* c2 = new TCanvas("c2","BPM Pedestals");
	c1->Divide(2,2);
	c2->Divide(2,2);
	TH1F *H[8];
	TF1 *f[8];

	int counts[8];


///// BPM wire loop!
	for(int i =0; i<8;i++){
		if(i<4){H[i] = new TH1F(Form("h%d",i),Form("BPMA %d",i+1),100,3200,4000);
			c1->cd(i+1); t->Draw(Form("%crb.BPMA.rawcur.%d>>h%d",side,i+1,i));}
		else{H[i] = new TH1F(Form("h%d",i),Form("BPMB %d",i-4),100,3200,4000);
			c2->cd(i-3); t->Draw(Form("%crb.BPMB.rawcur.%d>>h%d",side,i-3,i));}
			counts[i]=H[i]->GetEntries();
			if(counts[i]<=0){cout << "There are zero counts in the first histogram. \n";
				cout << "Please double check the name of the tree branches. \n";
				cout <<endl<<endl;exit(1);return; }
		f[i]= new TF1(Form("f%d",i), "gaus",3200,4000);	
		H[i]->Fit(Form("f%d",i),"Q","",3200,4000);
		pedestal(i)=f[i]->GetParameter("Mean");
		H[i]->GetXaxis()->SetRangeUser(pedestal(i)-100,4000);
		c1->Update();}
///////////////////////////////////////////////////////////////////////////////

//Output statments for Peds.
	cout<<" Please change the pedestals for BPMA to:"<<endl;
	for (Int_t i=0; i<4; i++) { cout<<" "<<(Int_t)pedestal(i)<<" ";}
	cout <<endl;
	cout<<" Please change the pedestals for BPMB to:"<<endl;
	for (Int_t i=4; i<8; i++) { cout<<" "<<(Int_t)pedestal(i)<<" ";}
	cout <<endl;
///////////////////////////////////////////////////////////////////////////////

//Saving pedestals to file:
//Keeping all ped values in one file.
	char output_file[100];
	int n = sprintf(output_file, "%BPM_Pedestals.txt");
	ifstream ifile(output_file);
	if (ifile) {ofstream myfile; 
		myfile.open("BPM_Pedestals.txt",std::fstream::in|std::fstream::out|td::fstream::app);
		myfile<<Run<<"  ";
		for (Int_t i=0; i<8; i++){myfile << pedestal(i) <<" ";}
		myfile<< endl;}
	else{ofstream myfile; 
		myfile.open("BPM_Pedestals.txt",std::fstream::in|std::fstream::out|td::fstream::app);
		myfile <<"Run     ";		
		for (Int_t i=0; i<4; i++){myfile <<"BPMA " << i+1<<"  ";}
		for (Int_t i=0; i<4; i++){myfile <<"BPMB " << i+1<<"  ";}	
		myfile <<endl;	myfile<<Run<<"  ";
		for (Int_t i=0; i<8; i++){myfile << pedestal(i) <<" ";}
		myfile<< endl;}


cout <<endl<<endl;exit(1);
}
