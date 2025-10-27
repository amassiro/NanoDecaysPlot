// printLHEDecay.C
// Compile and run with ROOT, e.g.:
// root -l -b -q 'printLHEDecay.C("NanoAOD.root", 3)'

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

//--------------------------------------------------
// Helper: map PDG IDs to human-readable particle names
//--------------------------------------------------
string pdgName(int pdgId) {
    static map<int, string> pdgNames = {
        {11, "e-"}, {-11, "e+"},
        {13, "mu-"}, {-13, "mu+"},
        {15, "tau-"}, {-15, "tau+"},
        {12, "νe"}, {-12, "νe̅"},
        {14, "νμ"}, {-14, "νμ̅"},
        {16, "ντ"}, {-16, "ντ̅"},
        {21, "g"},
        {22, "γ"},
        {23, "Z"},
        {24, "W+"}, {-24, "W-"},
        {25, "H"},
        {6, "t"}, {-6, "t̅"},
        {5, "b"}, {-5, "b̅"},
        {4, "c"}, {-4, "c̅"},
        {3, "s"}, {-3, "s̅"},
        {2, "u"}, {-2, "u̅"},
        {1, "d"}, {-1, "d̅"}
    };
    auto it = pdgNames.find(pdgId);
    if (it != pdgNames.end()) return it->second;
    return "PDG(" + to_string(pdgId) + ")";
}


//--------------------------------------------------
// 2) Read GenPart info and print Higgs decays
//--------------------------------------------------
void printGenDecay(const char *fname = "NanoAOD.root", int maxEvents = 5) {
    TFile *file = TFile::Open(fname);
    if (!file || file->IsZombie()) {
        cerr << "Error: could not open file " << fname << endl;
        return;
    }

    TTree *t = (TTree*)file->Get("Events");
    if (!t) {
        cerr << "Error: could not find TTree 'Events' in file " << fname << endl;
        return;
    }

    UInt_t nGenPart;
    const int maxN = 2000;
    int   GenPart_pdgId[maxN];
    int   GenPart_status[maxN];
    int   GenPart_genPartIdxMother[maxN];
    float GenPart_pt[maxN];
    float GenPart_eta[maxN];
    float GenPart_phi[maxN];
    float GenPart_mass[maxN];

    t->SetBranchAddress("nGenPart", &nGenPart);
    t->SetBranchAddress("GenPart_pdgId", GenPart_pdgId);
    t->SetBranchAddress("GenPart_status", GenPart_status);
    t->SetBranchAddress("GenPart_genPartIdxMother", GenPart_genPartIdxMother);
    t->SetBranchAddress("GenPart_pt", GenPart_pt);
    t->SetBranchAddress("GenPart_eta", GenPart_eta);
    t->SetBranchAddress("GenPart_phi", GenPart_phi);
    t->SetBranchAddress("GenPart_mass", GenPart_mass);

    Long64_t nentries = t->GetEntries();
    int nToRead = min<int>(nentries, maxEvents);

    cout << "\n=== Gen Higgs Decay Summary ===\n";

    for (int i = 0; i < nToRead; ++i) {
        t->GetEntry(i);
        cout << "\n--- Event " << i << " ---\n";

        // Find all Higgs bosons
        vector<int> higgsIndices;
        for (int j = 0; j < nGenPart; ++j) {
            if (GenPart_pdgId[j] == 25) higgsIndices.push_back(j);
        }

        if (higgsIndices.empty()) {
            cout << "No Higgs boson in this event.\n";
            continue;
        }

        for (int idx : higgsIndices) {

            // Collect its direct daughters
            vector<int> daughters;
            for (int k = 0; k < nGenPart; ++k) {
                if (GenPart_genPartIdxMother[k] == idx)
                    daughters.push_back(k);
            }

            if (daughters.empty()) {
                cout << "  No daughters found (possibly intermediate copy)\n";
                continue;
            }

            if (daughters.size()==1) {
                // Higgs intermediate state
                continue;
            }
            cout << "Higgs found: index " << idx
                 << ", pt=" << GenPart_pt[idx]
                 << ", eta=" << GenPart_eta[idx] << "\n";

            cout << "  H -> ";
            for (size_t k = 0; k < daughters.size(); ++k) {
                int pdg = GenPart_pdgId[daughters[k]];
                cout << pdgName(pdg);
                if (k + 1 < daughters.size()) cout << " + ";
            }
            cout << endl;
        }
    }

    file->Close();
}
