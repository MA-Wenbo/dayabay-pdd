//
//  Grapher.c
//  
//
//  Created by Leo Lu on 2022/10/15.
//

#include <stdio.h>

void grapher() {

    ////////////////////
    // pre-operations //
    ////////////////////

    // open the file
    auto data_file = TFile::Open("output/pair_81408.root", "read");

    // read the Event tree from the file
    auto tree = (TTree *)data_file->Get("ibd_tree"); // main detector
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
    
    if (true) {
        // plot energy, range from 0 to 15 MeV, divide into 300 bins
        auto c1 = new TCanvas("c1");       // declare a new canvas
        c1->SetLogy(1);                    // set log y axis for the canvas
        Event->Draw("energy2>>(100,6,10)"); // plot
//        Event->Draw("energy1>>(300,0,15)");
        c1->SaveAs("review_plots/delay_plots/plot 1.pdf");           // save as the first page of plot.pdf

        // plot x y of events
        c1->SetLogy(0);                                                  // set no log y
        Event->Draw("y2:x2>>(100,-3000,3000,100,-3000,3000)", "", "colz"); // plot
        c1->SaveAs("review_plots/delay_plots/plot 2.pdf");                                          // save as the next page of plot.pdf

        // plot radius-square of events
        Event->Draw("y2^2+x2^2>>(100,0,5e6)"); // plot
        c1->SaveAs("review_plots/delay_plots/plot 3.pdf");              // save

        // plot z vs radius-square of events
        Event->Draw("z2:y2^2+x2^2>>(50,0,5e6,50,-3000,3000)", "", "colz"); // plot
        c1->SaveAs("review_plots/delay_plots/plot 4.pdf");                                         // save
        // save
        
        Event->Draw("t2", "", "hist text0"); // plot
        c1->SaveAs("review_plots/delay_plots/plot 5.pdf");
        
        Event->Draw("t1:x1^2+y1^2", "", "colz"); // plot
        c1->SaveAs("review_plots/delay_plots/plot 6.pdf");
        
    }
    if(true){
        // plot energy, range from 0 to 15 MeV, divide into 300 bins
        auto c1 = new TCanvas("c1");       // declare a new canvas
        c1->SetLogy(1);                    // set log y axis for the canvas
        Event->Draw("energy1>>(100,0,4)"); // plot
//        Event->Draw("energy1>>(300,0,15)");
        c1->SaveAs("review_plots/prompt_plots/plot 1.pdf");           // save as the first page of plot.pdf

        // plot x y of events
        c1->SetLogy(0);                                                  // set no log y
        Event->Draw("y1:x1>>(100,-3000,3000,100,-3000,3000)", "", "colz"); // plot
        c1->SaveAs("review_plots/prompt_plots/plot 2.pdf");                                          // save as the next page of plot.pdf

        // plot radius-square of events
        Event->Draw("y1^2+x1^2>>(100,0,5e6)"); // plot
        c1->SaveAs("review_plots/prompt_plots/plot 3.pdf");              // save

        // plot z vs radius-square of events
        Event->Draw("z1:y1^2+x1^2>>(50,0,5e6,50,-3000,3000)", "", "colz"); // plot
        c1->SaveAs("review_plots/prompt_plots/plot 4.pdf");                                         // save
        
        Event->Draw("t1", "", "hist text0"); // plot
        c1->SaveAs("review_plots/prompt_plots/plot 5.pdf");
        
        Event->Draw("t1:x1^2+y1^2", "", "colz"); // plot
        c1->SaveAs("review_plots/prompt_plots/plot 6.pdf");
        
        Event->Draw("t2*1e6-t1*1e6", "", "hist"); // plot
        c1->SaveAs("review_plots/plot delta t.pdf");
    }
}
