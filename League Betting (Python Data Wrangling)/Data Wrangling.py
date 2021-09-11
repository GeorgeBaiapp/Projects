from bs4 import BeautifulSoup
import urllib.request
import pandas as pd
from datetime import date

# These are the teams for the League of Legends World Championship, from Korea.
# NA teams:'Cloud9', '100_Thieves', 'Team_Liquid'. EU teams: 'MAD_Lions', 'Fnatic', 'Rogue'
teams = ['DWG_KIA', 'T1', 'Gen.G', 'Hanwha_Life_Esports']


# This grabs a page by treating the user as a real person rather than a bot, in order to get around
# rate limiting from websites.
def grab_page(url):
    request = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
    page = urllib.request.urlopen(request).read()
    soup = BeautifulSoup(page, 'html.parser')

    return soup


# This doesn't work properly at the current moment.
# Need to fix!
# The problem is that it takes into account too many players. However, some of these players are not pro players
# so searching for these players will only lead to issues down the road.
def grab_players(worlds):
    players = []

    for teams in worlds:
        team_url = 'https://lol.fandom.com/wiki/' + teams
        team_soup = grab_page(team_url)
        team_player = team_soup.findAll("td", attrs={"class":"team-members-player"})

        for player in team_player:
            players.append(player.get_text())

    return players


# This scrapes the amount of games played by an account within the last 30 days.
# This works well, but doesn't properly take into account FBI, huhi, Elyoyaaa, Ratón calvo, Carzzy bob kong and Blaber
# This is because names are not necessarily uniform across websites, so what will work for one website will
# Not work for another.
def grab_games(url):
    soup = grab_page(url)

    results = soup.find(id="graphDD2")
    if results.string is None:
        results = soup.find(id="graphDD3")

    games_played = int(results.string)

    return games_played, soup


# This takes a pro player's account and finds out how many solo queue games
# they've played in total over the past 30 days
def all_games(url, player):
    # player needs to be exactly the right upper and lowercase
    # There might be a problem with people that have both other accounts and friends to play with though
    # But testing this with Canyon, it doesn't seem to matter
    games_played, soup = grab_games(url)
    other_accounts = soup.find("div", class_="box box-padding-10")
    searcher = player + "'s other accounts"

    # Checks that the pro player has other accounts and if so, checks how many games they have played and then
    # adds it to the total
    if other_accounts is not None and searcher in str(other_accounts.find("h3", class_="box-title")):
        other_accounts = other_accounts.findAll('a', href=True)
        for accounts in other_accounts:
            new_url = 'https://www.leagueofgraphs.com' + accounts['href'] + '/last-30-days'
            games_played += grab_games(new_url)[0]

    return games_played


# Problem With Willer -> He was just subbed in
# Alphari = Unknown account.
# There is one in comments and one that is not in comments because different websites use different usernames

DWG_players = ['Khan', 'Canyon', 'ShowMaker', 'Ghost', 'BeryL']
GENG_players = ['Rascal', 'Clid', 'Bdd', 'Ruler', 'Life']
T1_players = ['Canna', 'Oner', 'Faker', 'Gumayusi', 'Keria']
HLE_players = ['Morgan', 'Willer', 'Chovy', 'Deft', 'Vsta']
HUNDRED_players = ['Ssumday', 'Closer', 'Abbedagge', 'FBI', 'huhi']
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

# These are all the players that need to be considered for the worlds matches.
ALL = DWG_players + GENG_players + T1_players + HLE_players + HUNDRED_players + TL_players + C9_players\
      + MAD_players + FNATIC_players + ROGUE_players


def create_data(players):
    games_dict = {}

    for player in players:
        print(player)
        url = 'https://www.leagueofgraphs.com/search/' + player
        soup = grab_page(url)
        main_account = 'https://www.leagueofgraphs.com' + soup.findAll('a')[-3].get('href') + '/last-30-days'
        # This gets the pro-player id. Very susceptible to changes in the website though.
        games_dict[player] = all_games(main_account, player)

    return games_dict


# This grabs how many games a player has played in the last 30 days
# at a given date in time.
def grab_games_played(player_list, sheet):
    games_dict = create_data(player_list)

    # This grabs the current moment at time
    now = date.today().strftime("%d-%m-%Y")

    data = {'player': games_dict.keys(), 'last30daysgames{}'.format(now): games_dict.values()}
    player_games = pd.DataFrame.from_dict(data)
    player_games.to_excel("{}.xlsx".format(sheet))


# The problem is in collecting the champion pool data from ssumday, huhi, upset, Beryl, and hans sama
# This is due to differences across websites in how they format their information.
def grab_champion_pool(player_list):
    champion_pool_dict = {}
    for players in player_list:
        url = 'https://gol.gg/players/list/season-S11/split-Summer/tournament-ALL/'
        soup = grab_page(url)

        # This finds the url for a given pro player whereupon their champion stats can be checked
        other_accounts = soup.find('a', {'title': '{} stats'.format(players)})
        # If there is a problem with finding the statistics for a person, this just continues to ensure that the
        # program will continue to capture data, and the rest can be filled out by hand.
        if other_accounts is None:
            continue
        else:
            other_accounts = other_accounts['href']

        # This is where the champion stats come from.
        new_url = str('https://gol.gg/players' + other_accounts[1:])
        new_soup = grab_page(new_url)

        table = False

        # This should only occur once, since we are finding a specific piece of text from the website which
        # tells us that we are at the right table to be scraping from.
        for caption in new_soup.find_all('caption'):
            if caption.get_text() == '{} champion pool.'.format(players):
                table = caption.find_parent('table', {
                    'class': ['table_list', 'footable', 'toggle-square-filled' 'footable-loaded' 'phone breakpoint']})

        if not table:
            continue

        table = table.find("tbody")
        champion_pool_dict[players] = len(table) - 2

    data = {'player': champion_pool_dict.keys(), 'champion_pool': champion_pool_dict.values()}
    champ_pool = pd.DataFrame.from_dict(data)
    champ_pool.to_excel("champion_pool.xlsx")


# This combines champion pool data and games played data using the pro name as a key
# This need to be generalised much more later so that it's more powerful
def combine_data(sheet_one, sheet_two):
    f1 = pd.read_excel("{}.xlsx".format(sheet_one))
    f2 = pd.read_excel("{}.xlsx".format(sheet_two))
    f3 = f1[["player", "last30daysgames01-09-2021"]].merge(f2[["player", "champion_pool"]], on="player", how="left")
    f3.to_excel("Mixed.xlsx", index=False)


# When names are grabbed from the gol.gg datasheet, they don't actually match with the names
# grabbed from other websites. This fixes the names so that they are the same
def fix_names(sheet_one, sheet_two):
    # Data I've collected myself through this program, as well as data collected from the gol.gg website
    collected_data = pd.read_excel("{}.xlsx".format(sheet_one))
    golgg_data = pd.read_excel("{}.xlsx".format(sheet_two))

    # Makes sure the golgg data for the player is in the form of a string so that it can match
    golgg_data.astype({"player": str})

    # For some reason, golgg has an empty whitespace at the end which I need to get rid of via this data wrangling
    golgg_data.player = golgg_data.player.apply(lambda x: x[:-1])

    analysis_data = collected_data[["player", "last30daysgames01-09-2021", "champion_pool"]].merge(golgg_data, on="player", how="left")
    analysis_data.to_excel("analysis_data.xlsx", index=False)

# grab_champion_pool(ALL)
# grab_games_played(ALL, "proplay5")
# combine_data("proplay", "champion_pool")
# fix_names("Mixed", "golgg")


# TODO
# Currently, there is no way to add another date beyond running this program each day. Have this program run
# automatically each day and add the new data to the excel sheet




