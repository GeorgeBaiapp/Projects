import pandas as pd
from datetime import date

today = date.today()
#now = today.strftime("%d-%m-%Y")
#print(now)

#games_dict = {'Showmaker':125, 'Bdd':33, 'Faker':174}

# This code sets up the initial excel file
# last30daysgames{date} = games played within the last 30 dates at a certain date
#data = {'player':games_dict.keys(), 'last30daysgames{}'.format(now):games_dict.values()}
#test = pd.DataFrame.from_dict(data)
#test.to_excel("proplay.xlsx")

# This code adds a new column to the excel file.
# Eventually, this should happen automatically every 24 hours
new = pd.read_excel('proplay.xlsx', index_col=0)
#new['last30daysgames{}'.format(now)] = games_dict.values()
print(new)
# Todo convert this into an excel sheet

