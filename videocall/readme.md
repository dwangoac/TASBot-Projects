# TASBot projects #

## For trans_prerecorded.sh ##

To generate an r16m from a pre-recorded video, do the following:

./trans_prerecorded.sh <avi file> <palette interval>
Palette interval is how often to generate and transmit a new palette in frames, minimum of 1.

Output is LoZ_out.r16m

## For emotes.py ##

Requires python3

Code was created by Sersium Arkun and MediaMagnet

uses Twitch's api to pull emotes from twitch's servers.
[https://twitchemotes.com/apidocs](https://twitchemotes.com/apidocs)

emotes come in three sizes:

- small: 28x28
- medium: 56x56
- large: 112x112

the script will require one of these to work

after that is done you can pick a channel to pull emotes for Twitch Prime/Turbo use `--twitch-turbo--` as the channel name.
