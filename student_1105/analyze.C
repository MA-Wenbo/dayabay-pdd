void analyze() {

    ////////////////////
    // pre-operations //
    ////////////////////

    // open the file
    auto data_file = TFile::Open("data/run81408.root", "read");

    // read the Event tree from the file
    auto tree = (TTree *)data_file->Get("Event"); // main detector
    // auto tree_muon = (TTree *)data_file->Get("PoolMuon"); // outer detector

    tree->Show(0); // show branches of the tree
    // tree->Print(0); // show variable type of the tree

    // print number of events of the tree
    int total_events = tree->GetEntries();
    printf("total events: %d\n", total_events);

    // set an alias for the tree
    auto Event = tree;

    // set marker style for the tree
    Event->SetMarkerSize(2);
    Event->SetMarkerStyle(20);
    Event->SetMarkerColorAlpha(kRed, 0.5);

    int t0 = 1574392765;
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

    if (false) {
        // plot energy, range from 0 to 15 MeV, divide into 300 bins
        auto c1 = new TCanvas("c1");       // declare a new canvas
        c1->SetLogy(1);                    // set log y axis for the canvas
        Event->Draw("energy>>(300,0,12)"); // plot
        c1->SaveAs("review_plots/all_plots/plot 1.pdf");           // save as the first page of plot.pdf

        // plot x y of events
        c1->SetLogy(0);                                                  // set no log y
        Event->Draw("y:x>>(100,-3000,3000,100,-3000,3000)", "", "colz"); // plot
        c1->SaveAs("review_plots/all_plots/plot 2.pdf");                                          // save as the next page of plot.pdf

        // plot radius-square of events
        Event->Draw("y^2+x^2>>(100,0,5e6)"); // plot
        c1->SaveAs("review_plots/all_plots/plot 3.pdf");              // save

        // plot z vs radius-square of events
        Event->Draw("z:y^2+x^2>>(50,0,5e6,50,-3000,3000)", "", "colz"); // plot
        c1->SaveAs("review_plots/all_plots/plot 4.pdf");                                         // save

        // plot energy, range from 7 to 9 MeV, 0.1 MeV per bin
        Event->Draw("energy>>(20,0,12)", "energy>0.7 && energy<12", "hist text0"); // plot
        c1->SaveAs("review_plots/all_plots/plot 5.pdf");                                                // save

        // plot z vs radius-square of events
        Event->Draw("z:y^2+x^2>>(100,0,5e6,100,-3000,3000)", "energy>0.7 && energy<12", "colz"); // plot
        c1->SaveAs("review_plots/all_plots/plot 6.pdf");                                                               // save

        // plot x y of events
        Event->SetMarkerSize(1);
        Event->Draw("y:x>>(100,-3000,3000,100,-3000,3000)", "energy>6 && energy<12", "p"); // plot
        c1->SaveAs("review_plots/all_plots/plot 7.pdf");                                                           // save as the next page of plot.pdf

        // plot radius-square of events
        Event->Draw("y^2+x^2>>(100,0,5e6)", "energy>0.7 && energy<12"); // plot
        c1->SaveAs("review_plots/all_plots/plot 8.pdf");                                      // save

        Event->Draw("sec+nanosec*1e-9", "", "hist"); // plot
        c1->SaveAs("review_plots/all_plots/plot 9.pdf");
        
        Event->Draw("sec+nanosec*1e-9:x^2+y^2", "x^2+y^2<500", "colz"); // plot
        c1->SaveAs("review_plots/all_plots/plot 10.pdf");
        
        Event->Draw("sec+nanosec*1e-9:z", "", "colz"); // plot
        c1->SaveAs("review_plots/all_plots/plot 11.pdf");
        
        return;
    }

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

    Event->SetBranchAddress("sec", &sec);
    Event->SetBranchAddress("nanosec", &nanosec);
    Event->SetBranchAddress("energy", &energy);
    Event->SetBranchAddress("x", &x);
    Event->SetBranchAddress("y", &y);
    Event->SetBranchAddress("z", &z);
    Event->SetBranchAddress("det", &det);

    

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
    ti = (double)sec+nanosec*10e-9;
    
    int cnt = 0;
    
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
            energy1 = energy;
            energy2 = energy_delay;
            Event->GetEntry(i);
            x2 = x; y2 = y; z2 = z;
            Event->GetEntry(j);
            x1 = x; y1 = y; z1 = z;
            cout << "filled!" << endl;
            ++cnt;

            // print
            printf("event index: %d ", j);
            printf("energy: %f ", energy);
//            printf("Delta T: %f\n", (double)(t2-t1)*1e6);

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
//    ibd_tree->Draw("t2*1e6-t1*1e6", "", "hist");
    ibd_tree->Draw("energy1", "", "hist");
    fout->Close();


}

