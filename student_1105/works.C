void works() {

    ////////////////////
    // pre-operations //
    ////////////////////

    // open the file
    auto data_file = TFile::Open("data/run81408.root", "read");
    auto ad_file = TFile::Open("output/muons_81408.root", "read");

    // read the Event tree from the file
    auto tree = (TTree *)data_file->Get("Event"); // main detector
    auto adtree = (TTree *)ad_file->Get("ad_tree");
    auto tree_muon = (TTree *)data_file->Get("PoolMuon"); // outer detector

    tree->Show(0); // show branches of the tree
    // tree->Print(0); // show variable type of the tree

    // print number of events of the tree
    int total_events = tree->GetEntries();
    int total_ads = adtree->GetEntries();
    int total_mu = tree_muon->GetEntries();
    printf("total events: %d, %d, %d\n", total_events, total_ads, total_mu);

    // set an alias for the tree
    auto Event = tree;
    auto Muon = tree_muon;
    auto AD = adtree;

    // set marker style for the tree
    Event->SetMarkerSize(2);
    Event->SetMarkerStyle(20);
    Event->SetMarkerColorAlpha(kRed, 0.5);

    ///////////////////////
    // physical analysis //
    ///////////////////////
    // NOT HERE!!!!

    //
    int sec;
    int nanosec;
    float energy;
    float x;
    float y;
    float z;
    int det;
    double t;
    int msec;
    int mnanosec;

    Event->SetBranchAddress("sec", &sec);
    Event->SetBranchAddress("nanosec", &nanosec);
    Event->SetBranchAddress("energy", &energy);
    Event->SetBranchAddress("x", &x);
    Event->SetBranchAddress("y", &y);
    Event->SetBranchAddress("z", &z);
    Event->SetBranchAddress("det", &det);
    Muon->SetBranchAddress("msec", &msec);
    Muon->SetBranchAddress("mnanosec", &mnanosec);
    AD->SetBranchAddress("t", &t);

    int t0 = 1574392765;

    // output file and tree
    auto fout = TFile::Open("output/pair_81408.root", "recreate");
    auto ibd_tree = new TTree("ibd_tree", "");
    float energy1;
    float energy2;
    float t1;
    float t2;
    float x1, x2, y1, y2, z1, z2, det1, det2;
    ibd_tree->Branch("t1", &t1);
    ibd_tree->Branch("t2", &t2);
    ibd_tree->Branch("energy1", &energy1);
    ibd_tree->Branch("energy2", &energy2);
    // tree->Branch("x1", &x1);
    // tree->Branch("x2", &x2);
    
    int cnt = 0;
    for (int i = 0; i < total_events; ++i) {
        Event->GetEntry(i);
        
        double time = double (sec) + double(nanosec) * 1e-9;

//         printf("%d\n", sec);
//         printf("%d\n", nanosec);
//         printf("%f\n", energy);
//         printf("%f\n", x);
//         printf("%f\n", y);
//         printf("%f\n", z);
//         printf("%d\n", det);
//         printf("%f\n", time);
        
        //energy cut for delayed
        if (energy < 6 || energy > 12) continue;
        
        
        //Muon cutting
        t = -100;
        for (int k = 0; k<= total_ads; ++k) {
            adtree->GetEntry(k);
            if (((double)time-(double)t)<1e-3){
                break;
            }
        }
//        printf("Current event time: %f, AD time: %f, ", time-t0, t-t0);
        if ((t>time-1e-3) && (t<time)){
//            printf("Cutted AD on times %f and %f, index %d\n", time-t0, t-t0, i);
            continue;
        }
        

        //AD cutting
        double mt = -100;
        for (int w = 0; w<= total_mu; ++w) {
            Muon->GetEntry(w);
            mt = double(msec) + double(mnanosec)*1e-9;
            if (((double)time-(double)mt)<6e-4){
//                printf("\n %f %f \n", time-t0, mt-t0);
                break;
            }
        }
        printf("event time: %f, muon time: %f\n", time-t0, mt-t0);
        if ((mt>time-6e-4) && (mt<time)){
            printf("Cutted muon on times %f and %f, index %d\n", time-t0, mt-t0, i);
            continue;
        }
        
        
    
//        printf("Gd-n IBD is found!\n");
//        printf("event index: %d ", i);
//        printf("energy: %f ", energy);
//        printf("time: %f\n", time);

        // save delay signal info
        double event_index_delay = i;
        double time_delay = time;
        double energy_delay = energy;
        int n_candidate_prev = 0;

        // initialize output variables
        t1 = 0;
        t2 = 0;
        energy1 = 0;
        energy2 = 0;
        
        // multiplicity cut for delayed
        bool flag = false;
        for (int j = event_index_delay + 1; j <= total_events; ++j){
            Event->GetEntry(j);
            double time_multid = double(sec) + double(nanosec) * 1e-9;
            double energy_multid = energy;
            if (time_multid - time_delay > 200e-6)
                break;
            if (energy_multid > 0.7){
                flag = true;
                break;
            }
        }
        if (flag) continue;
        
//        printf("Survived multiplicity cut!\n");
        
        // time-backward search
        for (int j = event_index_delay - 1; j >= 0; --j) {
            Event->GetEntry(j);
            double time_prompt = double(sec) + double(nanosec) * 1e-9;
//            cout << time_prompt << endl;
            if (time_delay - time_prompt > 200e-6)
                break;
            
            //energy cut for prompt
            if (energy < 0.7 || energy > 12)
                continue;
            
            //multiplicity cut for prompt
            bool flag = false;
            for (int k = event_index_delay - 1; k >= 0; --k){
                if (k==j) continue;
                Event->GetEntry(k);
                double time_multip = double(sec) + double(nanosec) * 1e-9;
                double energy_multip = energy;
                if (time_prompt - time_multip > 200e-6)
                    break;
                if (energy_multip > 0.7){
                    flag = true;
                    break;
                }
            }
            if (flag)
                continue;
            
            // found prompt signal
            t1 = time_prompt;
            t2 = time_delay;
            energy1 = energy;
            energy2 = energy_delay;
            if (false){
                cout << "IBD Candidate found!" << endl;
                // print
                printf("event index: %d ", j);
                printf("energy: %f ", energy);
                printf("time: %.12f\n", time_prompt);
            }

            n_candidate_prev++;
            cnt++;
        }

        // check number of candidate prompt signals
        if (n_candidate_prev == 1)
            ibd_tree->Fill();
    }

    fout->cd();
    ibd_tree->Write();
    fout->Close();
    printf("\n%d\n", cnt);

}
