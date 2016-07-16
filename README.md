# Console Shuffle
A simple Windows implementation of Brian Barto's [No More Secrets](https://github.com/bartobri/no-more-secrets)

## Build
No extra library needed. All Windows compilers (mingw or Visual Studio) should work.
```
g++ -o shuffle shuffle.cpp -O2
```

## Feature
The program "encrypts" the current screen buffer of the console and displays it on the screen. It pauses until you press enter to start the decryption. Once it is done, press enter again to exit.
```
dir & shuffle
```

## Command Line Options
`-a` flag eliminates the need to press a key to start the decryption. 

`-e` and `-d` flags change the color of the encrypted and decrypted text respectively. The operand after these two flags is the same as in `COLOR` command of command prompt. By default, encrypted text is in white while decrypted text is in light blue, default background color is black.
```
shuffle -e 07 -d 01 # this is the same as default setting
shuffle -d 0E       # change decrypted text to light yellow with black background
```
Confused? Try `COLOR /?`.

## Limitation
Only works with Ascii code.
