#include "crosscheck.hh"

void crosscheck() {

    TFile *data_file = TFile::Open("data/run81408.root", "read");
    TTree *event_tree = (TTree *)data_file->Get("Event");        // main detector
    TTree *pool_muon_tree = (TTree *)data_file->Get("PoolMuon"); // main detector
    int total_events = event_tree->GetEntries();
    printf("total events: %d\n", total_events);

    EventTree Event(event_tree);
    PoolMuonTree PoolMuon(pool_muon_tree);

    Event.tree->GetEntry(0);
    PoolMuon.tree->GetEntry(0);

    int t0 = std::min(Event.sec, PoolMuon.msec);

    TDatime td_t0(t0);
    cout << "[Run start time] " << t0 << endl;
    cout << "[Run start time] ";
    td_t0.Print();

    int Nsingle = 0; // single nGd
    int Npair = 0;

    TFile *fout = TFile::Open("output/pair_81408.root", "recreate");
    PairTree pair_tree("pair_tree", "pair tree 81408 v22/11/05");

    vector<AdEvent> vmuad;
    vector<AdEvent> vmush;

    for (int i = 0; i < total_events; ++i) {
        Event.tree->GetEntry(i);

        // ad muon
        if (Event.energy > 20 && Event.energy < 2500) {
            auto muad = Event.GetEvent(i);
            auto muWsVeto = PoolMuon.CalcMuWsVeto(muad);
            if (muWsVeto[0] < 2e-6 || muWsVeto[1] < 2e-6) {
                vmuad.push_back(muad); // save mu_ad event
            }
        }
        // ad muon shower
        if (Event.energy > 2500) {
            auto mush = Event.GetEvent(i);
            auto muWsVeto = PoolMuon.CalcMuWsVeto(mush);
            if (muWsVeto[0] < 2e-6 || muWsVeto[1] < 2e-6) {
                vmush.push_back(mush); // save mu_ad event
            }
        }
        // n-Gd
        if (Event.energy > 6 && Event.energy < 10) {
            auto delay = Event.GetEvent(i);
            vector<AdEvent> pair = {delay};
            cout << "i: " << i << " ";
            cout << "Ed: " << Event.energy << " ";
            auto vprompt = Event.SearchPrompt(i);
            int Nmulti = 0;
            if (vprompt.size() == 0) {
                // single nGd event
                cout << endl;
                Nmulti = Event.SearchMultiplicity(delay.i, delay.i, total_events);
                Nsingle++;
            } else {
                // paired events
                for (const auto &prompt : vprompt) {
                    cout << "Ep: " << prompt.energy << " ";
                    cout << "Tp: " << GetTimeDiff(prompt, delay) * 1e6 << " " << endl;
                }
                Nmulti = Event.SearchMultiplicity(vprompt.front().i, delay.i, total_events);
                pair.insert(pair.begin(), vprompt.begin(), vprompt.end());
                Npair++;
            }
            // check water pool muon veto
            auto muWsVeto = PoolMuon.CalcMuWsVeto(delay);
            pair_tree.wpMuVeto = muWsVeto[0] > 600e-6 ? true : false;
            pair_tree.tPrevMuWs = muWsVeto[0];
            pair_tree.tPostMuWs = muWsVeto[1];

            // check ad muon veto
            if (vmuad.size() > 0) {
                auto tdiff = GetTimeDiff(vmuad.back(), delay);
                pair_tree.adMuVeto = tdiff > 1000e-6 ? true : false;
                pair_tree.tPrevAdMu = tdiff;
            } else {
                pair_tree.adMuVeto = true;
                pair_tree.tPrevAdMu = -1;
            }

            // check ad shower muon veto
            if (vmush.size() > 0) {
                auto tdiff = GetTimeDiff(vmush.back(), delay);
                pair_tree.adShVeto = tdiff > 1 ? true : false;
                pair_tree.tPrevAdSh = tdiff;
            } else {
                pair_tree.adShVeto = true;
                pair_tree.tPrevAdSh = -1;
            }

            pair_tree.Nmulti = Nmulti;
            pair_tree.WriteEvent(pair);
        }
    }

    auto c1 = new TCanvas("c1");
    pair_tree.tree->Draw("energy[0]", "Nprompt==1 && Nmulti==0");
    c1->SaveAs("crosscheck.pdf(");

    pair_tree.tree->Draw("energy[1]", "Nprompt==1 && Nmulti==0");
    c1->SaveAs("crosscheck.pdf");

    pair_tree.tree->Draw("dt", "Nprompt==1 && Nmulti==0");
    c1->SaveAs("crosscheck.pdf");

    pair_tree.tree->Draw("dr", "Nprompt==1 && Nmulti==0");
    c1->SaveAs("crosscheck.pdf)");
    fout->cd();
    pair_tree.tree->Write();
    fout->Close();

    cout << endl;
    cout << "number of single nGd events: " << Nsingle << endl;
    cout << "number of paired events: " << Npair << endl;

    // output file and tree
    // auto fout = TFile::Open("output/test_81408.root", "recreate");
    // auto ibd_tree = new TTree("ibd_tree", "");
    // float t1;
    // ibd_tree->Branch("t1", &t1);
}
