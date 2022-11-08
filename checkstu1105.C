{
	TFile *_file0 = TFile::Open("student_1105/output/pair_81408.root");
	auto ibd_tree = (TTree*)_file0->Get("ibd_tree");
    auto c1 = new TCanvas("c1");
    ibd_tree->Draw("energy1");
    c1->SaveAs("checkstu1105.pdf(");
    ibd_tree->Draw("energy2");
    c1->SaveAs("checkstu1105.pdf");
    ibd_tree->Draw("1e6*(t2-t1)");
    c1->SaveAs("checkstu1105.pdf");
    ibd_tree->Draw("sqrt((x1-x2)^2+(y1-y2)^2+(z1-z2)^2)");
    c1->SaveAs("checkstu1105.pdf)");
}