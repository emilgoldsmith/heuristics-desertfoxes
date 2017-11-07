def want(artist):
  if standings[mybidderid][artist] == 0:
    add = 0
    if numberbidders <= 3:
      add = 1
    rd = len(winnerarray)
    cnt = 0
    ran = min(gameLength-rd, max(20-rd, 10))
    for i in range(rd+1, rd+1+ran):
      if itemsinauction[i] == artist:
        cnt += 1
    if cnt >= (ran+3)//4 + 1:
      return 2 + add
    if cnt < ran//4:
      return 0 + add
    else:
      return 1 + add

def numberOutbidders(threat, x, us = '', rounds = 0):
  outbids = 0
  avgMinus = 15/numberbidders
  for opponent in players:
    if opponent != threat and opponent != us:
      outbids += (standings[opponent]['money']-avgMinus*rounds)//(x+1)
  return outbids

def maxMoney(threat):
  ret = 0
  for opponent in players:
    if opponent != threat:
      ret = max(standings[opponent]['money'], ret)
  return ret

def allowedSpend(threat):
  return standings[mybidderid]['money']-(maxMoney(threat)+1)//2 - 1

def threatRange(threat, target, rd):
  threatList = []
  for opponent in players:
    if opponent != threat:
      flag = False
      for artist in artists:
        if standings[opponent][artist] == 2 and flag == False:
          threatList.append(opponent)
          flag = True
        
  ret = gameLength
  for opponent in threatList:
    outbids = numberOutbidders(opponent, standings[opponent]['money'], mybidderid)
    cnt = 0
    rd = len(winnerarray)
    while cnt <= outbids and rd < gameLength:
      if standings[opponent][itemsinauction[rd]] == 2:
        cnt += 1
      rd += 1
    ret = min(ret, rd)
  return ret

def winningStrategy(threat, target, rd, bidOnTarget):
  maxRd = threatRange(threat, target, rd)
  rounds = 0
  while itemsinauction[rd + rounds + 1] != target:
    rounds += 1
  outbids = numberOutbidders(threat, standings[threat]['money']-bidOnTarget, '', rounds)
  cnt = 0
  while rd < maxRd:
    if itemsinauction[rd] == target or standings[threat][itemsinauction[rd]] == 2:
      cnt += 1
    rd += 1
  if cnt > outbids:
    return True
  return False

def determinebid(itemsinauction, winnerarray, winneramount, numberbidders, players, artists, standings, rd):
  #Updating our guess at other people's standard bids
  if rd > 0 and winnerarray[rd-1] != mybidderid:
    if winneramount[rd-1] < 10:
      groupAmounts[1] = winneramount[rd-1] + 1
    elif winneramount[rd-1] < 20:
      groupAmounts[2] = winneramount[rd-1] + 1
    groupAmounts[3] = allowedSpend(mybidderid)//2
  print("updated groupAmounts is:", groupAmounts)


  current = itemsinauction[rd]
  for player in players:
    if standings[player][current] == 3:
      print("Game is over")
      return 0
  money = standings[mybidderid]['money']
  #check if it's impossible to win with current item
  if current in dontCare:
    print("We don't care")
    return 105
  #If we can win on this painting, bid everything
  if standings[mybidderid][current] == 2:
    print("All in")
    return money

  #outbid others
  maxMon = 0
  for player in players:
    if standings[player][current] == 2 and standings[player]['money'] > maxMon:
      maxMon = standings[player]['money']
  if maxMon > 0:
    bid = -1
    for target in artists:
      if standings[mybidderid][target] == 2:
        if winningStrategy(mybidderid, target, rd+1, 0):
          bid = 0
          break
    if standings[mybidderid][current] == 0 and bid == -1:
      bid = groupAmounts[want(current)]
    elif bid == -1:
      for bid in range(standings[mybidderid]['money'], -1, -1):
        if winningStrategy(mybidderid, current, rd+1, bid):
          break
    if maxMon + 1 < bid and money-bid > maxMon and numberbidders > 3:
      print("Overbidding aggresively")
      return min(money, maxMon+2)
    print('sleeping')
    time.sleep(4)
    print('overbidding')
    time.sleep(0.5)
    return min(money, maxMon+1)
  
  #Check if we already have a winning strategy
  for target in artists:
    if standings[mybidderid][target] == 2:
      if winningStrategy(mybidderid, target, rd+1, 0):
        print("We have a chance to win if we don't spend more")
        return 0

  #If we have one of the painting, check winning strategy
  if standings[mybidderid][current] == 1:
    for bid in range(standings[mybidderid]['money'], -1, -1):
      if winningStrategy(mybidderid, current, rd+1, bid):
        print("Going aggresively with winningStrategy")
        return bid

  #if we don't have any, check want function / find target for 2 and 3 players
  if numberbidders > 3:
    group = want(current)
    print("Going with group", group)
    return min(groupAmounts[group], money)
    print ("something went wrong")
    return min(money, 5)
  else:
    cnt = {painting: 0 for painting in artists}
    i = -1
    while (max(cnt.values()) < 4 and numberbidders == 3) or (max(cnt.values()) < 3 and numberbidders == 2):
      i += 1
      cnt[itemsinauction[i]] += 1
    print("Target is", itemsinauction[i])
    if rd <= i:
      if itemsinauction[i] == current:
        print("bidding aggresively on target")
        return groupAmounts[3]
      else:
        print("Ignoring since it's not target")
        return 0
    else:
      print("Target passed, using want function")
      return groupAmounts[want(current)]



groupAmounts = [3, 7, 17, 24];
gameLength = 0
count = {artist: 0 for artist in artists}
while max(list(count.values())) < ((2)*numberbidders+1):
	count[itemsinauction[gameLength]] += 1
	gameLength += 1
dontCare = []
for key in count:
	if count[key] < 3:
		dontCare.append(key)