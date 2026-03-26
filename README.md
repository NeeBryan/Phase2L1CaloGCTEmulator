# Phase2L1CaloGCTEmulator

Project overlay for Phase-2 L1 Calo GCT emulator development in CMSSW.

This repository contains the project-specific code used to develop and test the GCT emulator chain, including:

- `DataFormats/L1TCalorimeterPhase2`
- `L1Trigger/L1CaloTrigger`
- `L1Trigger/L1CaloPhase2Analyzer`

## Current development status

- [x] RCT output link reading
- [x] Pre-IP1 GCT input preparation
- [ ] IP1 implementation
- [ ] IP1 validation

The current checked-in emulator state corresponds to:

**pre-IP1 RCT-to-GCT link handling**

That means the code currently focuses on:
- reading the available RCT output links
- defining the GCT-side data format
- reorganizing the link content for later GCT processing

and **does not yet implement IP1**.

---

## Repository layout

```text
DataFormats/L1TCalorimeterPhase2/
L1Trigger/L1CaloTrigger/
L1Trigger/L1CaloPhase2Analyzer/
````

### Important note about `L1Trigger/L1TCalorimeter`

This repository does **not** include the full `L1Trigger/L1TCalorimeter` package contents.

For the CMSSW build and runtime environment to work correctly, you may need to add that package in your local CMSSW area with:

```bash
git cms-addpkg L1Trigger/L1TCalorimeter
```

This is intentional: the repository tracks the project-specific overlay code, not the full CMSSW package set.

---

## Prerequisites

This project is intended to be used inside a CMSSW release area.

Example release used during development:

```bash
CMSSW_15_0_0_pre3
```

You should also have:

* a working CMSSW environment
* access to `git cms-init` / `git cms-addpkg`
* a successful `cmsenv`

---

## Fresh setup

### 1. Create the CMSSW release area

```bash
cmsrel CMSSW_15_0_0_pre3
cd CMSSW_15_0_0_pre3/src
cmsenv
```

### 2. Clone this repository into `src`

```bash
git clone https://github.com/NeeBryan/Phase2L1CaloGCTEmulator.git .
```

### 3. Check out the project branch

Right now the active development branch is:

```bash
git checkout project-overlay
```

### 4. Add the required CMSSW package dependency

```bash
git cms-addpkg L1Trigger/L1TCalorimeter
```

### 5. Build

```bash
scram b -j 8
```

If you update code later, rebuild with:

```bash
scram b -j 8
```

---

## Existing development area setup

If you already have the CMSSW release area and want to use this overlay:

```bash
cd /path/to/CMSSW_15_0_0_pre3/src
cmsenv
git clone https://github.com/NeeBryan/Phase2L1CaloGCTEmulator.git .
git checkout project-overlay
git cms-addpkg L1Trigger/L1TCalorimeter
scram b -j 8
```

---

## Main files in this repository

### GCT emulator plugin

```text
L1Trigger/L1CaloTrigger/plugins/Phase2L1CaloL1GCTEmulator.cc
```

This is the main emulator plugin under development.

### GCT emulator producer config

```text
L1Trigger/L1CaloTrigger/python/l1tPhase2GCTEmulatorProducer_cfi.py
```

This defines the CMSSW configuration fragment for the GCT emulator producer.

### GCT chain test config

```text
L1Trigger/L1CaloTrigger/test/test_gct_chain.py
```

This is the main test configuration used to run the GCT chain.

### GCT output data format

```text
DataFormats/L1TCalorimeterPhase2/interface/GCT_output.h
```

This defines the project-specific data format used for the GCT emulator output.

### Analyzer package

```text
L1Trigger/L1CaloPhase2Analyzer/
```

This package contains analyzer code, helper scripts, and plotting/test utilities used in the workflow.

---

## Building the project

From inside the CMSSW `src` area:

```bash
cmsenv
scram b -j 8
```

If you want a clean rebuild:

```bash
scram b clean
scram b -j 8
```

---

## Running the test chain

The main test config currently tracked in this repo is:

```bash
L1Trigger/L1CaloTrigger/test/test_gct_chain.py
```

Run it with:

```bash
cmsRun L1Trigger/L1CaloTrigger/test/test_gct_chain.py
```

If your config expects specific local input files, update the file paths inside the python config before running.

A useful way to save the full runtime log is:

```bash
cmsRun L1Trigger/L1CaloTrigger/test/test_gct_chain.py > log_test_gct_chain.txt 2>&1
```

---

## Analyzer workflow

The analyzer package is included in:

```text
L1Trigger/L1CaloPhase2Analyzer/
```

Depending on the analyzer/config you want to run, typical usage is:

```bash
cmsRun <some analyzer cfg.py>
```

or project-specific scripts inside:

```text
L1Trigger/L1CaloPhase2Analyzer/test/
```

Please inspect the relevant test/config file before running, since some scripts may assume specific local paths, inputs, or datasets.

---

## Development workflow

### Check current status

```bash
git status
```

### Stage changes

```bash
git add <files>
```

### Commit changes

Recommended commit style for this project:

```text
gct: <short milestone description>
```

Examples:

```text
gct: add pre-IP1 RCT-to-GCT link handling
gct: implement IP1 for GCT emulator input stage
gct: validate IP1 output with test chain
gct: fix RCT link ordering for IP1 inputs
```

### Push changes

```bash
git push
```

---

## Current branch structure

The main development branch currently used in this repository is:

```bash
project-overlay
```

If you clone the repository and want the current active development state, make sure you switch to that branch.

---

## Notes for collaborators

This repository is meant to provide the **project overlay**, not a full standalone CMSSW distribution.

So the expected usage is:

1. create the correct CMSSW release
2. enter the `src` directory
3. clone this repository into `src`
4. add missing CMSSW packages as needed via `git cms-addpkg`
5. build with `scram`

At the moment, the main extra package dependency that may need to be added manually is:

```bash
git cms-addpkg L1Trigger/L1TCalorimeter
```

---

## Troubleshooting

### Build fails because a package is missing

Make sure you added:

```bash
git cms-addpkg L1Trigger/L1TCalorimeter
```

Then rebuild:

```bash
scram b -j 8
```

### Runtime config cannot find files

Check the input file paths inside the relevant python config.

### Old build artifacts cause problems

Try:

```bash
scram b clean
scram b -j 8
```

---

## Recommended first commands after cloning

```bash
cmsrel CMSSW_15_0_0_pre3
cd CMSSW_15_0_0_pre3/src
cmsenv
git clone https://github.com/NeeBryan/Phase2L1CaloGCTEmulator.git .
git checkout project-overlay
git cms-addpkg L1Trigger/L1TCalorimeter
scram b -j 8
cmsRun L1Trigger/L1CaloTrigger/test/test_gct_chain.py
```

---
