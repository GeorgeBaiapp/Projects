from bs4 import BeautifulSoup
import urllib.request
import csv
import re

#LCK = ['DWG_KIA', 'T1', 'Nongshim_RedForce', 'Gen.G', 'Afreeca_Freecs', 'Fredit_BRION', 'Liiv_SANDBOX', 'DRX', 'KT_Rolster', 'Hanwha_Life_Esports']

# Todo modularise repeated code further
# Also make more robust to changes

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

    return players\


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

midlaners = ['ShowMaker', 'Bdd', 'Faker']

def create_data(players):
    games_dict = {}
    for player in players:
        url = 'https://www.leagueofgraphs.com/search/' + player
        req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
        page = urllib.request.urlopen(req).read()
        soup = BeautifulSoup(page, 'html.parser')
        main_account = 'https://www.leagueofgraphs.com' + soup.findAll('a')[-3].get('href') + '/last-30-days'
        # This gets the pro-player id. Very susceptible to changes in the website though.
        games_dict[player] = all_games(main_account, player)

    return games_dict

games_dict = create_data(['Canyon'])
print(games_dict)