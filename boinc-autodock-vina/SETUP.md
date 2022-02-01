# Prerequisites to run work-generator

# ADFR Suite install

```
wget https://ccsb.scripps.edu/adfr/download/1038/ -O $HOME/ADFRsuite_x86_64Linux_1.0.tar.gz --no-check-certificate
tar zxvf $HOME/ADFRsuite_x86_64Linux_1.0.tar.gz -C $HOME
dir=$(pwd)
cd $HOME/ADFRsuite_x86_64Linux_1.0/
yes | ./install.sh -d $HOME/ADFRsuite -c 0
cd $dir
```

Add to .bashrc:
```
export PATH=~/ADFRsuite/bin:"$PATH"
```

# OpenBabel

```
sudo apt-get install openbabel libopenbabel-dev swig
sudo ln -s /usr/include/openbabel3/ /usr/local/include/openbabel3
```

# RDKit

```
sudo apt-get install python3-rdkit librdkit1 rdkit-data
```

# Meeko

```
pip3 install -U numpy openbabel meeko rdkit-pypi
```

Add to .bashrc:
```
export PATH=~/.local/bin/:"$PATH"
```

# Scipy

```
pip3 install -U scipy matplotlib ipython jupyter pandas sympy nose
```
