from bs4 import BeautifulSoup
import urllib.request
import pandas as pd
from datetime import date

teams = ['DWG_KIA', 'T1', 'Gen.G', 'Hanwha_Life_Esports']
#, 'Cloud9', '100_Thieves', 'Team_Liquid', 'MAD_Lions', 'Fnatic', 'Rogue'

# Todo modularise repeated code further
# Also make more robust to changes

# This doesn't work
# Todo fix maybe
def grab_players(worlds):
    players = []

    for teams in worlds:
        team_url = 'https://lol.fandom.com/wiki/' + teams
        team_req = urllib.request.Request(team_url, headers={'User-Agent': 'Mozilla/5.0'})
        team_page = urllib.request.urlopen(team_req).read()
        team_soup = BeautifulSoup(team_page, 'html.parser')
        team_player = team_soup.findAll("td", attrs={"class":"team-members-player"})

        for player in team_player:
            players.append(player.get_text())

    return players


def grab_games(url):
    req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})

    page = urllib.request.urlopen(req).read()
    soup = BeautifulSoup(page, 'html.parser')

    # This scrapes the amount of games played within the last 30 days.
    results = soup.find(id="graphDD2")
    if results.string is None:
        results = soup.find(id="graphDD3")
    games_played = 0
    #print(url)

    # FBI, huhi, Elyoyaaa, Ratón calvo, Carzzy bob kong, Blaber
    #print(results)
    games_played = int(results.string)

    return games_played, soup


# This takes a pro player's account and finds out how many solo queue games
# they've played in total over the past 30 days
def all_games(url, player):
    # player  needs to be exactly the right upper and lowercase
    # There might be a problem with people that have both other accounts and friends to play with though
    # But testing this with Canyon, it doesn't seem to matter
    games_played, soup = grab_games(url)
    other_accounts = soup.find("div", class_="box box-padding-10")
    searcher = player + "'s other accounts"
    if other_accounts is not None and searcher in str(other_accounts.find("h3", class_="box-title")):
        other_accounts = other_accounts.findAll('a', href=True)
        for accounts in other_accounts:
            new_url = 'https://www.leagueofgraphs.com' + accounts['href'] + '/last-30-days'
            games_played += grab_games(new_url)[0]

    return games_played

DWG_players = ['Khan', 'Canyon', 'ShowMaker', 'Ghost', 'BeryL']
GENG_players = ['Rascal', 'Clid', 'Bdd', 'Ruler', 'Life']
T1_players = ['Canna', 'Oner', 'Faker', 'Gumayusi', 'Keria']
# Problem With Willer -> He was just subbed in
HLE_players = ['Morgan', 'Willer', 'Chovy', 'Deft', 'Vsta']
HUNDRED_players = ['Ssumday', 'Closer', 'Abbedagge', 'FBI', 'huhi']
# Alphari = Unknown account
TL_players = ['Alphari', 'Santorin', 'Jensen', 'Tactical', 'CoreJJ']
C9_players = ['Fudge', 'Blaber', 'Perkz', 'Zven', 'Vulcan']
MAD_players = ['Armut', 'Elyoya', 'Humanoid', 'Carzzy', 'Kaiser']
FNATIC_players = ['Adam', 'Bwipo', 'Nisqy', 'Upset', 'Hylissang']
ROGUE_players = ['Odoamne', 'Inspired', 'Larssen', 'Hans%20sama', 'Trymbi']
'''
DWG_players = ['Khan', 'Canyon', 'ShowMaker', 'Ghost', 'BeryL']
GENG_players = ['Rascal', 'Clid', 'Bdd', 'Ruler', 'Life']
T1_players = ['Canna', 'Oner', 'Faker', 'Gumayusi', 'Keria']
# Problem With Willer -> He was just subbed in
HLE_players = ['Morgan', 'Willer', 'Chovy', 'Deft', 'Vsta']
HUNDRED_players = ['ssumday', 'Closer', 'Abbedagge', 'FBI', 'Huhi']
# Alphari = Unknown account
TL_players = ['Alphari', 'Santorin', 'Jensen', 'Tactical', 'CoreJJ']
C9_players = ['Fudge', 'Blaber', 'Perkz', 'Zven', 'Vulcan']
MAD_players = ['Armut', 'Elyoya', 'Humanoid', 'Carzzy', 'Kaiser']
FNATIC_players = ['Adam', 'Bwipo', 'Nisqy', 'UpseT', 'Hylissang']
ROGUE_players = ['Odoamne', 'Inspired', 'Larssen', 'Hans sama', 'Trymbi']
'''
ALL = DWG_players + GENG_players + T1_players + HLE_players + HUNDRED_players + TL_players + C9_players + MAD_players + FNATIC_players + ROGUE_players

def create_data(players):
    games_dict = {}
    for player in players:
        print(player)
        url = 'https://www.leagueofgraphs.com/search/' + player
        req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
        page = urllib.request.urlopen(req).read()
        soup = BeautifulSoup(page, 'html.parser')
        main_account = 'https://www.leagueofgraphs.com' + soup.findAll('a')[-3].get('href') + '/last-30-days'
        # This gets the pro-player id. Very susceptible to changes in the website though.
        games_dict[player] = all_games(main_account, player)

    return games_dict


# need to learn to replace values
# also the error was that the last part was screwed need lambda x: x[:-1]
'''
f1 = pd.read_excel("Mixed.xlsx")
f2 = pd.read_excel("test.xlsx")
f2.astype({"player": str})
f2.player = f2.player.apply(lambda x: x[:-1])
f3 = f1[["player", "last30daysgames01-09-2021", "champion_pool"]].merge(f2, on = "player", how = "left")
f3.to_excel("Final.xlsx", index = False)
# Here I just deleted the unnecessary rows but this is suspect.
'''
'''
f1 = pd.read_excel("ta.xlsx")
f1 = f1.astype({"player": str})
f1 = f1[f1['player'].apply(lambda x: any([k in str(x) for k in ALL])) != False]

f1.to_excel("test.xlsx", index = False)
'''

#f1 = f1[f1["player"].isin(['ssumday', 'Upset'])]
#print(f1)
'''
f1 = pd.read_excel("proplay.xlsx")
f2 = pd.read_excel("champion_pool.xlsx")
f3 = f1[["player", "last30daysgames01-09-2021"]].merge(f2[["player", "champion_pool"]], on = "player", how = "left")
f3.to_excel("Mixed.xlsx", index = False)
'''
'''
# okay yeah the problem is ssumday, huhi, upset and hans sama. Beryl is missing as well
champion_pool_dict = {}
for players in ALL:
    url = 'https://gol.gg/players/list/season-S11/split-Summer/tournament-ALL/'
    req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
    page = urllib.request.urlopen(req).read()
    soup = BeautifulSoup(page, 'html.parser')


    other_accounts = soup.find('a', {'title' : '{} stats'.format(players)})
    if other_accounts is None:
        print('{} stats'.format(players))
        continue
    else:
        other_accounts = other_accounts['href']
    new_url = str('https://gol.gg/players' + other_accounts[1:])
    new_req = urllib.request.Request(new_url, headers={'User-Agent': 'Mozilla/5.0'})
    new_page = urllib.request.urlopen(new_req).read()
    new_soup = BeautifulSoup(new_page, 'html.parser')


    for caption in new_soup.find_all('caption'):
        #print(caption.get_text())
        if caption.get_text() == '{} champion pool.'.format(players):
            table = caption.find_parent('table', {'class': ['table_list', 'footable', 'toggle-square-filled' 'footable-loaded' 'phone breakpoint']})

    table = table.find("tbody")
    champion_pool_dict[players] = len(table) - 2

data = {'player': champion_pool_dict.keys(), 'champion_pool':champion_pool_dict.values()}
test = pd.DataFrame.from_dict(data)
test.to_excel("champion_pool.xlsx")
print(test)
'''

games_dict = create_data(ALL)
#games_dict = create_data(MAD_players + C9_players)

today = date.today()
now = today.strftime("%d-%m-%Y")
data = {'player':games_dict.keys(), 'last30daysgames{}'.format(now):games_dict.values()}
test = pd.DataFrame.from_dict(data)
test.to_excel("proplay2.xlsx")
print(test)
