int t0 = 1574392765;

void muon() {
    
    ////////////////////
    // pre-operations //
    ////////////////////
    
    // open the file
    auto data_file = TFile::Open("data/run81408.root", "read");
    
    // read the Event tree from the file
    auto tree = (TTree *)data_file->Get("Event"); // main detector
    auto mutree = (TTree *)data_file->Get("PoolMuon");
    // auto tree_muon = (TTree *)data_file->Get("PoolMuon"); // outer detector
    
    tree->Show(0); // show branches of the tree
    mutree->Show(0);
    // tree->Print(0); // show variable type of the tree
    
    // print number of events of the tree
    int total_events = tree->GetEntries();
    printf("total events: %d\n", total_events);
    
    int total_muons = mutree->GetEntries();
    printf("total muons: %d\n",total_muons);
    
    // set an alias for the tree
    auto Event = tree;
    auto Muon = mutree;
    
    //
    int sec;
    int nanosec;
    float energy;
    float x;
    float y;
    float z;
    int det;
    int msec;
    int mnanosec;
    int nHit;
    int mdet;
    
    Event->SetBranchAddress("sec", &sec);
    Event->SetBranchAddress("nanosec", &nanosec);
    Event->SetBranchAddress("energy", &energy);
    Event->SetBranchAddress("x", &x);
    Event->SetBranchAddress("y", &y);
    Event->SetBranchAddress("z", &z);
    Event->SetBranchAddress("det", &det);
    
    Muon->SetBranchAddress("msec", &msec);
    Muon->SetBranchAddress("mnanosec", &mnanosec);
    Muon->SetBranchAddress("nHit", &nHit);
    Muon->SetBranchAddress("mdet", &mdet);
    
    
    // output file and tree
    auto fout = TFile::Open("output/muons_81408.root", "recreate");
    auto ad_tree = new TTree("ad_tree", "");
    auto sh_tree = new TTree("sh_tree", "");
    float energym;
    float energye;
    int index;
    double tm;
    double te;
    //float x1, x2, y1, y2, z1, z2, det1, det2;
    ad_tree->Branch("t", &te);
    ad_tree->Branch("energy", &energye);
    sh_tree->Branch("t", &te);
    sh_tree->Branch("energy", &energye);
    ad_tree->Branch("index", &index);
    sh_tree->Branch("index", &index);
    // tree->Branch("x1", &x1);
    // tree->Branch("x2", &x2);
    
    //    printf("Det");
    int cnt = 0;
    
    for (int i = 0; i < total_events; ++i){
        Event->GetEntry(i);
        index = int(i);
        te = sec+nanosec*1e-9;
        energye = energy;
        //bool flag = false;
        //if (i%100==0) printf("%d ", i);
        
        Muon->GetEntry(cnt);
        tm = msec+mnanosec*1e-9;
        while(te-tm>2e-6 && cnt<total_muons){
            ++cnt;
            Muon->GetEntry(cnt);
            tm = sec+nanosec*1e-9;
        }
        
        if(tm>2e-6+te || tm<te-2e-6) continue;
        
        cnt = max(cnt-10, 0);
        
        if (energye>2500){
            printf("Cosmic Shower Muon Found!\n");
            printf("Event Index: %d", i);
            printf("\nTime after t0: ");
            cout<< tm-1574392765 <<endl;
            sh_tree->Fill();
        }
        else if (energye>20){
            printf("Antineutrino Detector Muon Found!\n");
            printf("Event Index: %d", i);
            printf("\nTime after t0: ");
            cout<< tm-1574392765 <<endl;
            ad_tree->Fill();
        }
    }
    
    fout->cd();
    sh_tree->Write();
    ad_tree->Write();
    fout->Close();
    
    return;
    
}
