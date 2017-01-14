import urllib.request
import os
import json

if not os.path.exists('./emotes'):
    os.makedirs('./emotes')
print('Saving emotes to folder: ' + os.path.abspath('./emotes') + '...')
print('Grabbing emote list...')
emotes = json.loads(urllib.request.urlopen('https://twitchemotes.com/api_cache/v2/global.json').read().decode("utf-8"))
for code, emote in emotes['emotes'].items():
    print('Downloading: ' + code + '...')
    urllib.request.urlretrieve(emotes['template']['large'].replace('{image_id}', str(emote['image_id'])),
                       './emotes/' + code + '.png')
print('Done pulling global emotes')
emotes = json.loads(urllib.request.urlopen('https://twitchemotes.com/api_cache/v2/subscriber.json').read().decode("utf-8"))
for emote in emotes['channels']['--twitch-turbo--']['emotes']:
    code = emote['code']
    print('Downloading: ' + code + '...')
    print(emotes['template']['large'].replace('{image_id}', str(emote['image_id'])),
                       './emotes/' + code + '.png')
    urllib.request.urlretrieve(emotes['template']['large'].replace('{image_id}', str(emote['image_id'])),
                       './emotes/' + code + '.png')
print('Done pulling twitch turbo emotes')
for emote in emotes['channels']['gamesdonequick']['emotes']:
    code = emote['code']
    print('Downloading: ' + code + '...')
    print(emotes['template']['large'].replace('{image_id}', str(emote['image_id'])),
                       './emotes/' + code + '.png')
    urllib.request.urlretrieve(emotes['template']['large'].replace('{image_id}', str(emote['image_id'])),
                       './emotes/' + code + '.png')
print('Done pulling GDQ emotes')
