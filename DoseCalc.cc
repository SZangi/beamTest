{
    TFile* F1 = new TFile("Only_DoseCup_Box1.root","read");
    TH1D* H1 = (TH1D*)F1->Get("DosimeternFlux");
    double dose = 0.;
    vector<double> doses = {7.70,10.2,12.7,17.3,21.5,27.2,35.2,42.5,54.7,75.0,92.8,108,116,130,178,220,250,272,282,290};
    int entries = doses.size();
    
    for (int i=0;i<entries;i++){dose += H1->GetBinContent(i)*doses[i];}

    double ratio = dose/H1->Integral();
    cout<<"Dose Ratio: "<< ratio<<endl;
    
}
