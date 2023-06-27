#!/usr/bin/env python3

# https://api.steampowered.com/ISteamApps/GetAppList/v2/

from markdown.extensions.tables import TableExtension
import markdown
import sys
from bs4 import BeautifulSoup
import subprocess
import json
import pandas as pd

f = open(sys.argv[1], "r")
text = f.read()
f.close()


html = markdown.markdown(text, extensions=[TableExtension(use_align_attribute=True)])

soup = BeautifulSoup(html, "html.parser")

h1 = ""
h2 = ""
h3 = ""

games = {}
h2Dict = {}
h3Dict = {}

listA = soup.contents
for x in listA:
	if not x.name == None:
		if x.name == "h1":
			h2 = ""
			h3 = ""
			h1 = x.contents[0]
			h2Dict = {}
		if x.name == "h2":
			h3 = ""
			h2 = x.contents[0]
			h3Dict = {}
		if x.name == "h3":
			h3 = x.contents[0]
		if x.name == "ul":
			content = []
			for item in x:
				if not item == "\n":
					content.append(item.contents[0].strip())

			if not h3 == "":
				h3Dict[h3] = content
				h2Dict[h2] = h3Dict
				games[h1] = h2Dict
			elif not h2 == "":
				h2Dict[h2] = content
				games[h1] = h2Dict
			else:
				games[h1] = content


fullGames = {"AAA": {}, "All VR": {}, "Base Building": {}, "Crafting": {}, "Creative Management": {}, "Platformer": {}, "Puzzle": {}, "Racing": {}, "RTS": {}, "Simulation": {}, "Small": {}, "Sports": {}, "Star Wars": {}, "Strategy": {}}

for platformA in games:
	for storeA in games[platformA]:
		for genreA, gamesListA in (games[platformA][storeA]).items():
			dictA = fullGames[genreA]
			for gameA in gamesListA:
				if gameA in fullGames[genreA].keys():
					fullGames[genreA][gameA]["platform"].append(platformA)
					fullGames[genreA][gameA]["store"].append(storeA)
				else:
					entryA = {
					"platform": [platformA],
					"store": [storeA],
					"hours": 0
					}
					dictA.update({gameA: entryA})
			fullGames[genreA] = dictA

fileName = "games.json"
if len(sys.argv) > 2:
	fileName = sys.argv[2]

with open(fileName, "w") as write_file:
	newJson = json.dump(fullGames, write_file, indent=2)

'''
f = open("steam.json", "r")
steamAPIString = f.read()
steamAPI = json.loads(steamAPIString)

listOfSteamGames = []
for x in fullGames.keys():
	for y in fullGames[x]:
		if "Steam" in fullGames[x][y]["store"]:
			listOfSteamGames.append(y)


ids = {}
for game in listOfSteamGames:
	found = False
	for x in steamAPI["applist"]["apps"]:
		toTest = x["name"].replace(" ™", "")
		toTest = toTest.replace("®", "")
	if toTest.replace("™", "") == game:
		ids.update({game: x["appid"]})
		found = True
	if not found:
		if game == "Grand Theft Auto: San Andreas":
			ids.update({game: "12120"})
		if game == "HelixVision":
			ids.update({game: "1137310"})
		if game == "FrostPunk":
			ids.update({game: "323190"})
		if game == "Rimworld":
			ids.update({game: "294100"})
		if game == "Burnout Paradise: The Ultimate Box":
			ids.update({game: "24740"})
		if game == "DIRT 3 Complete Edition":
			ids.update({game: "321040"})
		if game == "DIRT 4":
			ids.update({game: "421020"})
		if game == "F1 2011":
			ids.update({game: "44360"})
		if game == "F1 2012":
			ids.update({game: "208500"})
		if game == "F1 2014":
			ids.update({game: "226580"})
		if game == "F1 2015":
			ids.update({game: "286570"})
		if game == "F1 2016":
			ids.update({game: "391040"})
		if game == "F1 2017":
			ids.update({game: "515220"})
		if game == "F1 2018":
			ids.update({game: "737800"})
		if game == "MotoGP 15":
			ids.update({game: "355130"})
		if game == "Project CARS":
			ids.update({game: "234630"})
		if game == "Project CARS 2":
			ids.update({game: "378860"})
		if game == "Age of Empires II HD Edition":
			ids.update({game: "221380"})
		if game == "Age of Empires II Definitive Edition":
			ids.update({game: "813780"})
		if game == "Age of Empires III Complete Collection":
			ids.update({game: "105450"})
		if game == "Star Wars Empire at War: Gold Pack":
			ids.update({game: "32470"})
		if game == "BEAR, VODKA, STALINGRAD":
			ids.update({game: "1232130"})
		if game == "Cook, Serve, Delicious":
			ids.update({game: "247020"})
		if game == "HELLION":
			ids.update({game: "588210"})
		if game == "PAC-MAN Museum":
			ids.update({game: "1665130"})
		if game == "LEGO Star Wars: The Force Awakens":
			ids.update({game: "438640"})
		if game == "Star Wars Battlefront (Classic, 2004)":
			ids.update({game: "1058020"})
		if game == "Star Wars Knights of the Old Republic":
			ids.update({game: "32370"})
		if game == "Star Wars Knights of the Old Republic II: The Sith Lords":
			ids.update({game: "208580"})
		if game == "Star Wars Republic Commando":
			ids.update({game: "6000"})
		if game == "Star Wars: Shadow of the Empire":
			ids.update({game: "560170"})
		if game == "Star Wars: The Old Republic":
			ids.update({game: "1286830"})
		if game == "Bloons TD6":
			ids.update({game: "960090"})
		if game == "Magic: The Gathering - Duels of the Planeswalkers 2013":
			ids.update({game: "87330"})
		if game == "DIRT Rally":
			ids.update({game: "310560"})


notAllGames = False

for game in listOfSteamGames:
	if not game in ids:
		print(game)
		notAllGames = True

if notAllGames:
	print("Error")
	exit()

for x in fullGames.keys():
	for y in fullGames[x]:
		if "Steam" in fullGames[x][y]["store"]:
			game = y
		fullGames[x][y]["steamID"] = ids[y]

fileName = "games.json"
if len(sys.argv) > 2:
	fileName = sys.argv[2]

with open(fileName, "w") as write_file:
	newJson = json.dump(fullGames, write_file, indent=2)
'''
