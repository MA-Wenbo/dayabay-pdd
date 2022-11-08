struct AdEvent {
    int i;
    int sec;
    int nanosec;
    float energy;
    float x;
    float y;
    float z;
    int det;
};

// calculate time difference between the former and the later event [sec]
double GetTimeDiff(AdEvent e0, AdEvent e1) {
    int diff_sec = e1.sec - e0.sec;
    int diff_nanosec = e1.nanosec - e0.nanosec;
    double diff_t = double(diff_sec) + double(diff_nanosec) * 1e-9;
    return diff_t; // sec
}

class EventTree {
  public:
    EventTree(TTree *t);
    int total_events = 0;
    // ~EventTree();
    TTree *tree;
    int sec;
    int nanosec;
    float energy;
    float x;
    float y;
    float z;
    int det;
    AdEvent GetEvent(int i);
    vector<AdEvent> SearchPrompt(int i);
    int SearchMultiplicity(int i0, int i1, int total);
};

EventTree::EventTree(TTree *t) : tree{t} {
    tree->SetBranchAddress("sec", &sec);
    tree->SetBranchAddress("nanosec", &nanosec);
    tree->SetBranchAddress("energy", &energy);
    tree->SetBranchAddress("x", &x);
    tree->SetBranchAddress("y", &y);
    tree->SetBranchAddress("z", &z);
    tree->SetBranchAddress("det", &det);
    total_events = tree->GetEntries();
}

AdEvent EventTree::GetEvent(int i) {
    tree->GetEntry(i);
    AdEvent event = {i, sec, nanosec, energy, x, y, z, det};
    return event;
}

// search from (i) to (i-200us) for candidate prompt signals
vector<AdEvent> EventTree::SearchPrompt(int i) {
    // store n-Gd
    AdEvent delay = GetEvent(i);
    // store prompt events
    vector<AdEvent> vprompt;
    while (i > 0) {
        i--;
        tree->GetEntry(i);
        AdEvent prompt = GetEvent(i);
        if (GetTimeDiff(prompt, delay) > 200e-6)
            break;
        if (energy > 0.7 && energy < 12) {
            vprompt.insert(vprompt.begin(), prompt);
        } else {
            continue;
        }
    }
    return vprompt; // time-ordered prompt signals
}

int EventTree::SearchMultiplicity(int i0, int i1, int total) {
    AdEvent prompt = GetEvent(i0);
    AdEvent delay = GetEvent(i1);
    int N = 0; // number of multiplicity events
    int i = i0;
    // search in the [prompt - 200us, prompt] window
    while (i > 0) {
        i--;
        AdEvent event = GetEvent(i);
        if (GetTimeDiff(event, prompt) > 200e-6)
            break;
        if (event.energy > 0.7)
            N++;
    }
    // search in the [delay, delay + 200us] window
    i = i1;
    while (i < total - 1) {
        i++;
        AdEvent event = GetEvent(i);
        if (GetTimeDiff(delay, event) > 200e-6)
            break;
        if (event.energy > 0.7)
            N++;
    }
    return N;
}

struct PoolMuonEvent {
    int i; // a current location of the tree
    int msec;
    int mnanosec;
    int nHit;
    int mdet;
};

// the latter one subtract the former one
double GetTimeDiff(AdEvent ad, PoolMuonEvent pm) {
    int diff_sec = pm.msec - ad.sec;
    int diff_nanosec = pm.mnanosec - ad.nanosec;
    double diff_t = double(diff_sec) + double(diff_nanosec) * 1e-9;
    return diff_t; // sec
}

class PoolMuonTree {
  public:
    PoolMuonTree(TTree *t);
    int total_events = 0;
    // ~PoolMuonTree();
    TTree *tree;
    int index; // a current location of the tree
    int msec;
    int mnanosec;
    int nHit;
    int mdet;
    PoolMuonEvent GetEvent(int i);
    vector<double> CalcMuWsVeto(AdEvent ad);
    vector<int> SearchNearestMuon(AdEvent, int, int);
};

PoolMuonTree::PoolMuonTree(TTree *t) : tree{t} {
    tree->SetBranchAddress("msec", &msec);
    tree->SetBranchAddress("mnanosec", &mnanosec);
    tree->SetBranchAddress("nHit", &nHit);
    tree->SetBranchAddress("mdet", &mdet);
    total_events = tree->GetEntries();
}

PoolMuonEvent PoolMuonTree::GetEvent(int i) {
    tree->GetEntry(i);
    PoolMuonEvent event = {i, msec, mnanosec, nHit, mdet};
    return event;
}

vector<double> PoolMuonTree::CalcMuWsVeto(AdEvent ad) {
    vector<int> nearest_muon_index = SearchNearestMuon(ad, 0, total_events - 1);
    auto tl = GetTimeDiff(ad, GetEvent(nearest_muon_index[0]));
    auto tr = GetTimeDiff(ad, GetEvent(nearest_muon_index[1]));
    return {-tl, tr}; // sec. both are positive value
}

// binary search for the left- and right- nearnest muon
vector<int> PoolMuonTree::SearchNearestMuon(AdEvent ad, int l, int r) {
    int m = l + (r - l) / 2;
    if (r - l <= 1)
        return {l, r};
    auto pm = GetEvent(m);
    bool search_left = GetTimeDiff(ad, pm) > 0;
    if (search_left) {
        r = m;
        return SearchNearestMuon(ad, l, r);
    } else {
        l = m;
        return SearchNearestMuon(ad, l, r);
    }
    return {l, r};
}

class PairTree {
  public:
    PairTree(TString name, TString title);
    TTree *tree;

    int *sec = new int[2];
    int *nanosec = new int[2];
    float *energy = new float[2];
    float *x = new float[2];
    float *y = new float[2];
    float *z = new float[2];
    int *det = new int[2];

    float dr;
    float dt;
    int Nprompt;
    int Nmulti;

    double tPrevMuWs;
    double tPostMuWs;
    bool wpMuVeto = true;

    double tPrevAdMu;
    bool adMuVeto = true;

    double tPrevAdSh;
    bool adShVeto = true;
    void WriteEvent(vector<AdEvent> pair);
};

PairTree::PairTree(TString name, TString title) {
    tree = new TTree(name, title);
    tree->Branch("sec", sec, "sec[2]/I");
    tree->Branch("nanosec", nanosec, "nanosec[2]/I");
    tree->Branch("energy", energy, "energy[2]/F");
    tree->Branch("x", x, "x[2]/F");
    tree->Branch("y", y, "y[2]/F");
    tree->Branch("z", z, "z[2]/F");
    tree->Branch("det", det, "det[2]/I");

    tree->Branch("dr", &dr);
    tree->Branch("dt", &dt);
    tree->Branch("Nprompt", &Nprompt);
    tree->Branch("Nmulti", &Nmulti);

    tree->Branch("tPrevMuWs", &tPrevMuWs);
    tree->Branch("tPostMuWs", &tPostMuWs);
    tree->Branch("wpMuVeto", &wpMuVeto);
    
    tree->Branch("tPrevAdMu", &tPrevAdMu);
    tree->Branch("adMuVeto", &adMuVeto);
    
    tree->Branch("tPrevAdSh", &tPrevAdSh);
    tree->Branch("adShVeto", &adShVeto);
}

void PairTree::WriteEvent(vector<AdEvent> pair) {
    Nprompt = pair.size() - 1;
    vector<AdEvent> pdpair;
    if (pair.size() == 1) {
        AdEvent prompt = {0, 0, 0, 0, 0, 0, 0, 0};
        pdpair = {prompt, pair[0]};
    } else {
        pdpair = {pair.front(), pair.back()};
    }
    int i = 0;
    for (const auto &event : pdpair) {
        sec[i] = event.sec;
        nanosec[i] = event.nanosec;
        energy[i] = event.energy;
        cout << "WriteEvent: " << energy[i] << endl;
        x[i] = event.x;
        y[i] = event.y;
        z[i] = event.z;
        det[i] = event.det;
        i++;
    }
    dt = 1e6 * GetTimeDiff(pair.front(), pair.back()); // us
    dr = sqrt(pow(pair[0].x - pair[1].x, 2) + pow(pair[0].y - pair[1].y, 2) + pow(pair[0].z - pair[1].z, 2));
    tree->Fill();
    return;
}
