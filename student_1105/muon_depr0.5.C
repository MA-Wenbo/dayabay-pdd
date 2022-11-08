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
    double tm;
    double te;
    //float x1, x2, y1, y2, z1, z2, det1, det2;
    ad_tree->Branch("t", &te);
    ad_tree->Branch("energy", &energye);
    sh_tree->Branch("t", &te);
    sh_tree->Branch("energy", &energye);
    // tree->Branch("x1", &x1);
    // tree->Branch("x2", &x2);
    
//    printf("Det");
    int cnt = 0;
    
    for (int i = 0; i < total_events; ++i){
        Event->GetEntry(i);
        te = (float) sec+nanosec*1e-9;
        energye = energy;
        //bool flag = false;
        //if (i%100==0) printf("%d ", i);
        
        Muon->GetEntry(cnt);
        tm = (float) msec+mnanosec*1e-9;
        while(te-tm>2e-6 && cnt<total_muons){
            ++cnt;
            Muon->GetEntry(cnt);
            tm = (float) msec+mnanosec*1e-9;
        }
        
        cnt = max(cnt-10, 0);
        
        if(tm>2e-6+te || tm<te-2e-6) continue;
        
        if (energye>2500){
            printf("Cosmic Shower Muon Found!\n");
            printf("Event Index: %d", i);
            printf("\nTime after t0: ");
//            cout<< tm-1574392765 <<endl;
            cout << te << endl;
            sh_tree->Fill();
        }
        else if (energye>20){
            printf("Antineutrino Detector Muon Found!\n");
            printf("Event Index: %d", i);
            printf("\nTime after t0: ");
//            cout<< tm-1574392765 <<endl;
            cout << te << endl;
            ad_tree->Fill();
        }
    }
    
    fout->cd();
    sh_tree->Write();
    ad_tree->Write();
    fout->Close();
    
    return;
    
    
    
    /*for (int i = 0; i < total_events; ++i) {
        Muon->GetEntry(i);
        double time = double(sec - t0) + double(nanosec) * 1e-9;
        
        while (event_time < time){
            ++cnt;
            Event->GetEntry(cnt);
            event_time = double(sec - t0) + double(nanosec) * 1e-9;
        }
        
        while (event_time - time < 2e-6){
            ++xtra;
            Event->GetEntry(cnt+xtra);
            event_time = double(sec - t0) + double(nanosec) * 1e-9;
            if (
        }
        xtra = -1;
                
        if (energy > 6 && energy < 12) { // standard IBD search energy cut
            // int j = i;
            printf("Gd-n IBD is found!\n");
            printf("event index: %d ", i);
            printf("energy: %f ", energy);
            printf("time: %f\n", time);

            // save delay signal info
            double event_index_delay = i;
            double time_delay = time;
            double energy_delay = energy;
            int n_candidate_prev = 0;

            // intialize output variables
            t1 = 0;
            t2 = 0;
            energy1 = 0;
            energy2 = 0;

            // time-backward search
            for (int j = event_index_delay - 1; j >= 0; --j) {
                Event->GetEntry(j);
                double time_prompt = double(sec - t0) + double(nanosec) * 1e-9;
                cout << time_prompt << endl;
                if (time_delay - time_prompt > 200e-6)
                    break;

                // found prompt signal
                t1 = time_prompt;
                t2 = time_delay;
                energy1 = energy;
                energy2 = energy_delay;
                cout << "filled!" << endl;

                // print
                printf("event index: %d ", j);
                printf("energy: %f ", energy);
                printf("time: %.12f\n", time_prompt);

                n_candidate_prev++;
            }

            // check number of candidate prompt signals
            if (n_candidate_prev == 1)
                ibd_tree->Fill();
        }
    }

    

    // int events_roi = Event->Draw("sec:nanosec:energy:x:y:z:det", "", "goff"); // graphical off
    // double *sec = Event->GetVal(0);
    // double *nanosec = Event->GetVal(1);
    // double *energy = Event->GetVal(2);
    // double *x = Event->GetVal(3);
    // double *y = Event->GetVal(4);
    // double *z = Event->GetVal(5);

    // scan events from 7.8-8 MeV, deem as "golden-Gd"
    // Event->Scan("energy:x:y:z");

    ///////////////////////
    // close the program //
    ///////////////////////

    // close the canvas, and save plot.pdf
    // Event->Draw("energy", "energy==0");
    // c1->SaveAs("plot.pdf)");

    // delete pointers of tree, close the file
    // delete tree;
    // data_file->Close();
    // delete data_file;
     */
}

/*
int bs(double target){
    
}

int binarySearch(int l, int r, double x)
{
    if (r < l) {
        int mid = l + (r - l) / 2;
        
        Event->GetEntry(mid);
        double t = double(sec - t0) + double(nanosec) * 1e-9;
        // If the element is present at the middle
        // itself
        if (t == x)
            return mid;
 
        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (t > x)
            return binarySearch(arr, l, mid - 1, x);
 
        // Else the element can only be present
        // in right subarray
        return binarySearch(arr, mid + 1, r, x);
    }
 
    // We reach here when element is not
    // present in array
    return mid;
}*/
