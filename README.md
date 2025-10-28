build and install
-----------------

```shell
make CC=cc\
     STRIP=strip # optional
sudo make install
```

test via alsa-tools
-----------------

```shell
alias aplay="aplay -fS16_LE -r44100 --buffer-size=1024"
alias arecord="arecord -fS16_LE -r44100 --buffer-size=1024"
C4=261.63
E4=392.00
G4=329.63
C5=523.25
```

### sources
```shell
sp-sine -D1 -a0.5 | aplay -c2
sp-saw -D1 -a0.5 | aplay -c2
sp-square -D1 -a0.5 | aplay -c2
# sp-constant
```

### mixer
```shell
sp-constand -a0.5 | sp-mixer -D1\
                                <(sp-saw -f$C4)\
                                <(sp-saw -f$E4)\
                                <(sp-saw -f$G4)\
                                <(sp-saw -f$C5)\
                                | aplay
```

### channels
```shell
sp-sine -D1 | sp-split >(aplay -c1) /dev/null
sp-combine -D1\
              <(sp-sine -C1 -a0.5 -f500)\
              <(sp-sine -C1 -a0.5 -f1000)\
              | aplay -c2
```