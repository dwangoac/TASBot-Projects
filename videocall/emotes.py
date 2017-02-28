# Twitch Emotes puller
# Pulls emotes from twitchemotes api 
# code wrote by sersium, arkun, and mediamagnet
import urllib.request
import os
import json

def chanpull():
	#for channel subscriber emotes
	twiname = str(input("What channel would you like me to pull? (use --twitch-turbo-- for Twitch prime/turbo emotes) "))
	emotes = json.loads(urllib.request.urlopen('https://twitchemotes.com/api_cache/v2/subscriber.json').read().decode("utf-8"))
	for emote in emotes['channels'][twiname]['emotes']:
		code = emote['code']
		print('Downloading: ' + code + '...')
		print(emotes['template'][sizevar].replace('{image_id}', str(emote['image_id'])),
						   './emotes/' + code + '.png')
		urllib.request.urlretrieve(emotes['template'][sizevar].replace('{image_id}', str(emote['image_id'])),
						   './emotes/' + code + '.png')
	print('Done pulling ' + twiname + ' emotes')

if not os.path.exists('./emotes'):
	os.makedirs('./emotes')
print('Saving emotes to folder: ' + os.path.abspath('./emotes') + '...')
sizevar = str(input("Would you like the large emotes or the small emotes? "))
# for global emotes
print('Grabbing emote list...')
emotes = json.loads(urllib.request.urlopen('https://twitchemotes.com/api_cache/v2/global.json').read().decode("utf-8"))
for code, emote in emotes['emotes'].items():
	print('Downloading: ' + code + '...')
	urllib.request.urlretrieve(emotes['template'][sizevar].replace('{image_id}', str(emote['image_id'])),
                                   './emotes/' + code + '.png')
print('Done pulling global emotes')


while True:
	a = input("Would you like to pull subscriber emotes from a channel? ")
	if a=="yes" or a=="y":
		chanpull()
		continue
	elif a=="no" or a=="n":
		break
	else:
		print("Enter either yes/no")

