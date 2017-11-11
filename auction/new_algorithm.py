def getBid(rd, maximizerId, minimizerId, standings, currentArtist):
  if weWon:
    return True
  if theyWon:
    return False
  if SomeoneCanWin and othersHaveMoney:
    deductMoneyFromOthers
    return recurse
  elif SomeoneCanWin:
    if weCanWin:
      deductFromThem
    else:
      deductFromUs
    return recurse

  lo = 0
  hi = standings[maximizerId]['money'] // (numToWin - standings[minimizerId][currentArtist])
  limit = hi
  if hi == standings[minimizerId]['money']:
    lo = hi + 1
  while lo < hi:
    mid = lo + ((hi - lo) // 2)
    newStandings = deepCopy(standings)
    newStandings[minimizerId][currentArtist] += 1
    newStandings[minimizerId]['money'] -= mid
    if getBid(newStandings)[0]:
      lo = mid + 1
    else:
      hi = mid
  if lo == limit:
    return (False, lo)
  return (True, lo)

def determinebid(itemsinauction, winnerarray, winneramount, numberbidders, players, artists, standings, rd, numToWin):
  current = itemsinauction[rd]
  money = standings[mybidderid]['money']

  #If we can win on this painting, bid everything
  if standings[mybidderid][current] == numToWin - 1:
    print("All in")
    return money

  #check if it's impossible to win with current item
  if current in dontCare:
    print("We don't care")
    return 0

  strategicBid = getBid()

  #outbid others
  maxMoney = -1
  for player in players:
    if standings[player][current] == numToWin - 1:
      maxMoney = max(standings[player]['money'], maxMoney)
  if maxMoney > -1:
    if strategicBid > maxMoney + 1:
      # The painting is worth getting for more than just a simple outbid so we outbid aggressively
      return strategicBid
    # The only reason we're outbidding is to prevent someone else from winning, not because it's beneficial
    # so we sleep in order to hope that someone else outbids them before us and we don't have to waste the money

    # make this dependant on how much time left + number of rounds left before pidgeon hole principle
    time.sleep(5)
    return maxMoney + 1

  # Otherwise we just return what our search thinks is the best bet
  return strategicBid


gameLength = 0
count = {artist: 0 for artist in artists}
while max(list(count.values())) < ((2)*numberbidders+1):
	count[itemsinauction[gameLength]] += 1
	gameLength += 1
dontCare = []
for key in count:
	if count[key] < numToWin:
		dontCare.append(key)
