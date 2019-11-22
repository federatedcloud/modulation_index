# Post-Install Steps

## Change user password
```
passwd jovyan
```

## Make sure bash history gets saved
```
sudo chown jovyan ~/.bash_history
```

## Run makewisdom to improve speed of fftw
```
cd $PRESTO/src
make makewisdom
```

For more info, see https://github.com/scottransom/presto/blob/master/INSTALL

## Install Ripgrep (optional, but nice tool)
```
curl -LO https://github.com/BurntSushi/ripgrep/releases/download/11.0.2/ripgrep_11.0.2_amd64.deb
sudo dpkg -i ripgrep_11.0.2_amd64.deb
```

For more info, see https://github.com/BurntSushi/ripgrep#installation

# Python version note
This container has both python 2.7 and python 3.6 (or greater) available, and certain software packages work with each.
The default active python is python 3.  To change to python 2, run
```
source activate python2
```

To make it permanent, change the `bashrc` file, before build, to add this command at the bottom.

