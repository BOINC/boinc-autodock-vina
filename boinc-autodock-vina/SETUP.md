# Prerequisites to run work-generator

# ADFR Suite install
wget https://ccsb.scripps.edu/adfr/download/1028/ -O adfr_install --no-check-certificate
chmod a+x adfr_install
./adfr_install
tar zxvf ~/ADFRsuite-1.0/ADFRsuite_x86_64Linux_1.0.tar.gz
cd ADFRsuite_x86_64Linux_1.0/
yes | ./install.sh -d ~/ADFRsuite -c 0

Add to .bashrc:
export PATH=~/ADFRsuite/bin:"$PATH"

# OpenBabel
sudo apt-get install openbabel libopenbabel-dev swig

# RDKit
sudo apt-get install python-rdkit librdkit1 rdkit-data

# Meeko
pip3 install -U numpy openbabel meeko rdkit-pypi

Add to .bashrc:
export PATH=~/.local/bin/:"$PATH"

# Scipy
pip3 install -U scipy matplotlib ipython jupyter pandas sympy nose


