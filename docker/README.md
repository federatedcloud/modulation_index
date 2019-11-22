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


