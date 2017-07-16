# Information Regarding the Control Panel

The control panel relies on a simple TCP connection from the bot
process to this python script. The panel should be started BEFORE the
bot is run, and the bot process will connect to it once the bots are
setup. Messages to the bot process contain one mnemonic, or a three
character command that explains what action you are trying to do,
followed by a string of characters for the mnemonic parameters. Some
mnemonics do not require an argument to be specified.

In the control panel specify the mnemonic you wish to use by
prepending your message with a `/`. For example if you wish to select
the first bot that connected, you would type `/sel 0`, where sel is
the 3 character mnemonic and 0 is the parameter. If you do not type a
`/` before your message, `say` is used as the default. That is, `/say
Hello` and simply `Hello` will send the exact same message.

A list of mnemonics are found below. Note that it is possible for
different api bots to have more mnemonics, or even overwrite these
ones, but here are the basic, default ones.

`say` : Sends _arg_ to a chat, which could be either the room chat, a
whisper, or a community platform chat.

`tar` : Changes the target of the say command. If _arg_ is `r`, then
the output of the `say` mnemonic will be room, if it is `c` then it
will be in the chosen community platform chat.

`cmd` : Issues a chat command in the game. For example, in-game to
check online mods you would use /mod, so in this control panel to
issue the same command you would use `/cmd mod`. This does not work
for every in-game command such as `/who` or `/chat`, and that is why
there are mnemonics for these special cases.

`sel` : Selects a bot to use, if no _arg_, then it displays the number
of running bots. You can only issue commands to one bot at a time, so
the `sel` mnemonics let's you choose which one you want to target. The
bots are zero-indexed, so the range for _arg_ is 0 to (num_bots - `).

`bot` : Displays info about the selected bot, including it's username,
id, and it's currently residing room.

`cht` : Joins a community platform chat. For example, to join the
`#us` chat you would type `/cht #us`. The command will issue a warning
if the chat name does not start with a `#`, but note that Transformice
server will still accept it without it.

`who` : When the room is targeted, display a list of players in the
room. When a chat is targeted, display a list users in the chat.

`cap` : When registering accounts, a captcha will need to be
completed. Once the captcha information is recieved, it will output
the bitmap to the terminal using ASCII characters. Specifiy the four
leters you see as the _arg_ to complete the captcha.

