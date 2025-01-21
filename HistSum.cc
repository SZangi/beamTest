{
    TFile *F1 = new TFile("WriteT.root");
    TH1F *H1 = (TH1F *)F1->Get("DosimeternFlux");
    TH1F *H2 = (TH1F *)F1->Get("DosimeternFlux2");

    double sum = 0;
    for (int j=0; j<17; j++){
        sum += H1->GetBinContent(j);
    }
    cout << sum << endl;

    sum = 0;
    for (int j=0; j<17; j++){
        sum += H2->GetBinContent(j);
    }
    cout << sum << endl;
}