import pandas as pd
from bs4 import BeautifulSoup
import urllib.request
import csv
import re

games_dict = {}
players = []

# leagueofgraphs can be searched for pro players by name

LCK = ['DWG_KIA', 'T1', 'Nongshim_RedForce', 'Gen.G', 'Afreeca_Freecs', 'Fredit_BRION', 'Liiv_SANDBOX', 'DRX', 'KT_Rolster', 'Hanwha_Life_Esports']

def grab_players(region):
    players = []

    for teams in region:
        team_url = 'https://lol.fandom.com/wiki/' + teams
        team_req = urllib.request.Request(team_url, headers={'User-Agent': 'Mozilla/5.0'})
        team_page = urllib.request.urlopen(team_req).read()
        team_soup = BeautifulSoup(team_page, 'html.parser')
        team_player = team_soup.findAll("td", attrs={"class":"team-members-player"})

        for player in team_player:
            players.append(player.get_text())

    return players
# use {} to capture the whole suite of DK members

#LCK_players = grab_players(LCK)

LCK_players = ['Khan', 'Canyon', 'ShowMaker', 'Ghost', 'BeryL', 'Malrang', 'Rahel', 'RangJun', 'Chasy', 'Nuclear', 'Nuguri', 'BeryL', 'Flame', 'Hoit', 'Punch', 'Aries', 'Flame', 'Alive', 'Crush', 'Veritas', 'Iron', 'Parang', 'CooN', 'Try', 'Canna', 'Oner', 'Faker', 'Gumayusi', 'Keria', 'Zeus', 'Cuzz', 'Clozer', 'Teddy', 'Hoit', 'Ellim', 'Effort', 'Roach', 'Kuri', 'Gori', 'Rich', 'Peanut', 'Gori', 'deokdam', 'Kellin', 'Wayne', 'Bay', 'Juhan', 'Rascal', 'Clid', 'Bdd', 'Ruler', 'Life', 'Burdol', 'YoungJae', 'Flawless', 'Karis', 'Kellin', 'CuVee', 'Roach', 'Peanut', 'SeongHwan', 'Fly', 'Kuzan', 'Rich', 'Asper', 'Haru', 'CoreJJ', 'Mong', 'Ambition', 'Crown', 'Kiin', 'Dread', 'Fly', 'Leo', 'Lehends', 'Bang', 'MapSSi', 'Keine', 'Trap', 'Spirit', 'Mystic', 'Ben', 'Hoon', 'ALL IN', 'SS', 'Jelly', 'Senan', 'Ucal', 'Aiming', 'Jelly', 'Proud', 'Summit', 'Mowgli', 'Kuro', 'Kramer', 'TusiN', 'MaRin', 'Lindarang', 'SeongHwan', 'Mickey', 'Sangyoon', 'SnowFlower', 'ikssu', 'LirA', 'Hoya', 'UmTi', 'Lava', 'Hena', 'Delight', 'Chieftain', 'Yaharong', 'Mightybear', 'Oscar', 'Wizer', 'Croco', 'FireRain', 'Hena', 'Minit', 'Roi', 'Doil', 'Jiin', 'KronG', 'Hollow', 'Road', 'Destroy', 'FATE', 'HyBriD', 'Cube', 'BroCColi', 'Cookie', 'Alive', 'bbangje', 'HighDin', 'Start', 'Summit', 'Croco', 'FATE', 'Prince', 'Effort', 'Howling', 'OnFleek', 'Kingkong', 'Leo', 'Route', 'Kabbie', 'GorillA', 'Dove', 'Punch', 'Lonely', 'Joker', 'Wizer', 'Hollow', 'Ghost', 'Crush', 'Justice', 'DoRaoN', 'Totoro', 'Kingen', 'Pyosik', 'Jett', 'Taeyoon', 'Jun', 'Destroy', 'SOLKA', 'BAO', 'Becca', 'Doran', 'Chovy', 'Deft', 'Keria', 'Rascal', 'Cuzz', 'Naehyun', 'TusiN', 'Doran', 'Blank', 'Dove', '5kid', 'Noah', 'Harp', 'Zzus', 'GIDEON', 'Ucal', 'HyBriD', 'Blank', 'Bonnie', 'GIDEON', '5kid', 'Harp', 'Rebel', 'Castle', 'Vicla', 'Noah', 'Aiming', 'Ray', 'Smeb', 'SoHwan', 'bonO', 'Malrang', 'Kuro', 'TusiN', 'Kingen', 'Smeb', 'Score', 'UmTi', 'Bdd', 'PraY', 'SnowFlower', 'Gango', 'Zenit', 'Mia', 'Rush', 'PawN', 'Ucal', 'Deft', 'Mata', 'Ssun', 'Chance', 'Steal', 'Ssumday', 'Fly', 'Arrow', 'Hachani', 'IgNar', 'Edge', 'Nagne', 'Piccaboo', 'Mach', 'Fixer', 'Ares', 'Prime', 'Sage', 'Mimic', 'IkSSu', 'Potato', 'Hachani', 'DuDu', 'Morgan', 'Willer', 'yoHan', 'Chovy', 'Deft', 'Vsta', 'Arthur', 'Riss', 'CaD', 'Mask', 'Cheoni', 'Baut', 'Viper', 'Lehends', 'Mireu', 'CuVee', 'Haru', 'YeongJae', 'Lava', 'Zenit', 'Tempt', 'SoHwan', 'Tempt', 'Moojin', 'key', 'Sangyoon', 'Thal', 'bonO', 'Clever', 'Asper', 'Lindarang', 'SeongHwan', 'Brook', 'Mightybear', 'Kuzan']

# TODO
# go to 'https://www.leagueofgraphs.com/search/' + player
# find the padding
# then find a href again
# then put that url into all_games and create the dict!
# the problem is that this will likely cause 300 requests, so this might be problematic to do


def grab_games(url):
    req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})

    page = urllib.request.urlopen(req).read()
    soup = BeautifulSoup(page, 'html.parser')

    # This scrapes the amount of games played within the last 30 days.
    results = soup.find(id="graphDD2")
    games_played = int(results.string)

    return games_played, soup


# This takes a pro player's account and finds out how many solo queue games
# they've played in total over the past 30 days
def all_games(url):
    games_played, soup = grab_games(url)
    other_accounts = soup.find("div", class_="box box-padding-10").findAll('a', href=True)
    for accounts in other_accounts:
        new_url = 'https://www.leagueofgraphs.com' + accounts['href'] + '/last-30-days'
        games_played += grab_games(new_url)[0]

    return games_played


url = 'https://www.leagueofgraphs.com/summoner/kr/DK+ShowMaker/last-30-days'

# Find a way to collect a dict of all games played for a team
#print(all_games(url))
