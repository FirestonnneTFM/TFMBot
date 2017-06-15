# TFMBot

Framework for connecting an unofficial client to the official
Transformice game.

This is a work in progress, and uses POSIX system calls, so you can
probably compile use it on unix-like operating systems.

### Keys

Running the bot requires a binary file named `keys.bin` in the same
folder as the application. This is a folder that contains some magic
numbers that are recompiled into the official flash client every
couple of minutes or so.

The contents of this file in order are:

- A 20 byte key in which other keys are hashed from
- A 32 bit integer key that needs to be used on login
- A 16 bit integer that needs to be used during the handshake
- A variable length string that needs to be used for the handshake

I will not go into detail on how to obtain these, but once you do
encode them in the file, with the 32 bit and 16 bit integers BOTH
encoded as a **little** endian 32 bit integers. The string is encoded
in ASCII with a byte before to denote the length. See the
keys.bin.SAMPLE.md file to see an example.

### License

MIT Liscense for all code, provided without warranty. See LICENSE file
for more details.

Transformice is probably a trademark owned by Atlier801 SARL.