// printLHEDecay.C
// Usage example:
//   root -l -b -q 'printLHEDecay.C("NanoAOD.root", 5)'

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

//----------------------------------------------------------
// Small helper: translate PDG IDs into human-readable names
//----------------------------------------------------------
string pdgName(int pdgId) {
    static map<int, string> pdgNames = {
        {11, "e-"}, {-11, "e+"},
        {13, "mu-"}, {-13, "mu+"},
        {15, "tau-"}, {-15, "tau+"},
        {12, "ve"}, {-12, "~ve"},
        {14, "vmu"}, {-14, "~vmu"},
        {16, "vtau"}, {-16, "~vtau"},
        {22, "gamma"},
        {23, "Z"},
        {24, "W+"}, {-24, "W-"},
        {25, "Higgs"},
        {6, "t"}, {-6, "tbar"},
        {5, "b"}, {-5, "bbar"},
        {1, "d"}, {-1, "dbar"},
        {2, "u"}, {-2, "ubar"},
        {3, "s"}, {-3, "sbar"},
        {4, "c"}, {-4, "cbar"},
        {21, "g"}
    };
    auto it = pdgNames.find(pdgId);
    if (it != pdgNames.end()) return it->second;
    return "PDG(" + to_string(pdgId) + ")";
}

//----------------------------------------------------------
// Main function: read NanoAOD LHEPart and print decay info
//----------------------------------------------------------
void printLHEDecay(const char *fname = "NanoAOD.root", int maxEvents = 10) {
    TFile *file = TFile::Open(fname);
    if (!file || file->IsZombie()) {
        cerr << "Error: could not open file " << fname << endl;
        return;
    }

    TTree *t = (TTree*)file->Get("Events");
    if (!t) {
        cerr << "Error: could not find TTree 'Events' in " << fname << endl;
        return;
    }

    std::cout << " maxEvents = "<< maxEvents << std::endl;
    // NanoAOD LHEPart branches
    UInt_t nLHEPart;
    // const int maxN = 256; // max number of LHE particles per event
    const int maxN = 20; // max number of LHE particles per event
    float LHEPart_pt[maxN];
    float LHEPart_eta[maxN];
    float LHEPart_phi[maxN];
    float LHEPart_mass[maxN];
    int   LHEPart_pdgId[maxN];
    int   LHEPart_status[maxN];

    // Attach branches (flat NanoAOD structure)
    t->SetBranchAddress("nLHEPart", &nLHEPart);
    t->SetBranchAddress("LHEPart_pt", LHEPart_pt);
    t->SetBranchAddress("LHEPart_eta", LHEPart_eta);
    t->SetBranchAddress("LHEPart_phi", LHEPart_phi);
    t->SetBranchAddress("LHEPart_mass", LHEPart_mass);
    t->SetBranchAddress("LHEPart_pdgId", LHEPart_pdgId);
    t->SetBranchAddress("LHEPart_status", LHEPart_status);

    Long64_t nentries = t->GetEntries();
    cout << "Opened " << fname << " with " << nentries << " events." << endl;

    int nToRead = min<int>(nentries, maxEvents);
    for (int i = 0; i < nToRead; ++i) {
        t->GetEntry(i);
        cout << "\n================= Event " << i << " =================" << endl;
        cout << "nLHEPart = " << nLHEPart << endl;

        vector<int> incoming, outgoing;
        for (int j = 0; j < nLHEPart; ++j) {
            int pdg = LHEPart_pdgId[j];
            int status = LHEPart_status[j];
            string name = pdgName(pdg);

            cout << " [" << j << "::" << nLHEPart << "] "
                 << name
                 << " (pdgId=" << pdg
                 << ", status=" << status
                 << ", pt=" << LHEPart_pt[j]
                 << ", eta=" << LHEPart_eta[j]
                 << ", phi=" << LHEPart_phi[j]
                 << ", m=" << LHEPart_mass[j]
                 << ")\n";

            if (status < 0) incoming.push_back(j);
            else if (status > 0) outgoing.push_back(j);
        }

        // Print simple decay summary
        cout << "\nDecay process: ";
        for (size_t k = 0; k < incoming.size(); ++k) {
            cout << pdgName(LHEPart_pdgId[incoming[k]]);
            if (k + 1 < incoming.size()) cout << " + ";
        }
        cout << " -> ";
        for (size_t k = 0; k < outgoing.size(); ++k) {
            cout << pdgName(LHEPart_pdgId[outgoing[k]]);
            if (k + 1 < outgoing.size()) cout << " + ";
        }
        cout << endl;
    }

    cout << "\nProcessed " << nToRead << " events.\n";
    file->Close();
}


