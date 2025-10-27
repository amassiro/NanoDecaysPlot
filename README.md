# NanoDecaysPlot

Simple scripts to check:

- matrix element process
- gen level Higgs decay


To run (assuming a root file in local with nanoaod information, "nanoStep_231.root"):

    root -l printLHEDecay.C\(\"nanoStep_231.root\",20\)
    root -l printGenDecay.C\(\"nanoStep_231.root\",20\)


"20" here is the number of events to check

