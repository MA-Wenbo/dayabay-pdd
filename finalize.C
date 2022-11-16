void finalize() {

    ////////////////////
    // pre-operations //
    ////////////////////

    // open the file
    auto data_file = TFile::Open("data/run81408.root", "read");
    auto ad_file = TFile::Open("output/muons_81408.root", "read");

    // read the Event tree from the file
    auto tree = (TTree *)data_file->Get("Event"); // main detector
    auto mutree = (TTree *)data_file->Get("PoolMuon");
    auto adtree = (TTree *)ad_file->Get("ad_tree");
    auto shtree = (TTree *)ad_file->Get("sh_tree");

    tree->Show(0); // show branches of the tree
    // tree->Print(0); // show variable type of the tree

    // print number of events of the tree
    int total_events = tree->GetEntries();
    int total_muons = mutree->GetEntries();
    int total_ads = adtree->GetEntries();
    int total_shs = adtree->GetEntries();
    printf("total events: %d\n", total_events);
    printf("total muons: %d\n", total_muons);
    printf("total ADs: %d\n", total_ads);
    printf("total SHs: %d\n", total_shs);

    // set an alias for the tree
    auto Event = tree;
    auto Muon = mutree;
    auto Ad = adtree;
    auto Sh = shtree;

    int t0 = 1574392765;
    
    ///////////////////////
    // physical analysis //
    ///////////////////////
    // NOT HERE!!!!

    //
    int sec;
    int nanosec;
    float energy;
    
    int msec;
    int mnanosec;
    
    double ta;
    double ts;
    
    float x;
    float y;
    float z;
    int det;

    Event->SetBranchAddress("sec", &sec);
    Event->SetBranchAddress("nanosec", &nanosec);
    Event->SetBranchAddress("energy", &energy);
    Event->SetBranchAddress("x", &x);
    Event->SetBranchAddress("y", &y);
    Event->SetBranchAddress("z", &z);
    Event->SetBranchAddress("det", &det);
    
    Muon->SetBranchAddress("msec", &msec);
    Muon->SetBranchAddress("mnanosec", &mnanosec);
    Ad->SetBranchAddress("t", &ta);
    Sh->SetBranchAddress("t", &ts);
    

    // output file and tree
    auto fout = TFile::Open("output/pair_81408.root", "recreate");
    auto ibd_tree = new TTree("ibd_tree", "");
    float energy1;
    float energy2;
    double ti;
    double tf;
    double t1;
    double t2;
    float x1, x2, y1, y2, z1, z2, det1, det2;
    ibd_tree->Branch("t1", &t1);
    ibd_tree->Branch("t2", &t2);
    ibd_tree->Branch("energy1", &energy1);
    ibd_tree->Branch("energy2", &energy2);
    ibd_tree->Branch("x1", &x1);
    ibd_tree->Branch("y1", &y1);
    ibd_tree->Branch("z1", &z1);
    ibd_tree->Branch("x2", &x2);
    ibd_tree->Branch("y2", &y2);
    ibd_tree->Branch("z2", &z2);
    
    // tree->Branch("x1", &x1);
    // tree->Branch("x2", &x2);
    Event->GetEntry(0);
    ti = sec+nanosec*10e-9;
    
    int cnt = 0;
    
    int mnt = 0;
    int ant = 0;
    int shw = 0;
    
    for (int i = 0; i < total_events; ++i) {
        Event->GetEntry(i);
        
        double time = double(sec - t0) + double(nanosec) * 1e-9;

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
            double time_multid = double(sec - t0) + double(nanosec) * 1e-9;
            double energy_multid = energy;
            if (time_multid - time_delay > 200e-6)
                break;
            if (energy_multid > 0.7){
                flag = true;
                break;
            }
        }
        if (flag) continue;
        
        // muon cut for delayed
        Muon->GetEntry(mnt);
        double tm = msec+mnanosec*1e-9;
        while(time_delay-tm>600e-6 && mnt<total_muons){
            ++mnt;
            Muon->GetEntry(mnt);
            tm = msec+mnanosec*1e-9;
        }
        if(tm<600e-6+time_delay && tm>time_delay-600e-6){
            mnt = max(mnt-1000,0);
            printf("ABORT BY MUON");
            continue;
        }
        mnt = max(mnt-1000,0);
        
        //ad cut for delayed
        Ad->GetEntry(ant);
        while(ta<time_delay-1000e-6 && ant<total_ads){
            ++ant;
            Ad->GetEntry(ant);
        }
        if(ta<=1000e-6+time_delay && ta>=time_delay-1000e-6){
            ant = max(ant-1000,0);
            printf("ABORT BY AD");
            continue;
        }
        ant = max(ant-1000,0);
        
        //sh cut for delayed
        Sh->GetEntry(shw);
        while(ts<time_delay-1 && ant<total_ads){
            ++ant;
            Sh->GetEntry(shw);
        }
        if(ts<=1+time_delay && ts>=time_delay-1){
            ant = max(ant-1000,0);
            printf("ABORT BY SH");
            continue;
        }
        shw = max(shw-1000,0);
        
        // time-backward search
        for (int j = event_index_delay - 1; j >= 0; --j) {
            Event->GetEntry(j);
            double time_prompt = double(sec - t0) + double(nanosec) * 1e-9;
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
                double time_multip = double(sec - t0) + double(nanosec) * 1e-9;
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
            energy2 = energy_delay;
            Event->GetEntry(i);
            x2 = x; y2 = y; z2 = z;
            Event->GetEntry(j);
            x1 = x; y1 = y; z1 = z;
            cout << "filled!" << endl;
            energy1 = energy;
            ++cnt;

            // print
            printf("event index: %d ", j);
            printf("energy: %f ", energy1);
            printf("time: %.12f\n", time_prompt);

            n_candidate_prev++;
        }

        // check number of candidate prompt signals
        if (n_candidate_prev == 1)
            ibd_tree->Fill();
    }
    Event->GetEntry(total_events-1);
    tf = (float)sec+nanosec*10e-9;
    
    printf("%f", tf-ti);
    printf("\n%d\n", cnt);
    
    fout->cd();
    ibd_tree->Write();
    fout->Close();


}

