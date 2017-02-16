while 1:
  try:
    i = int(raw_input('? '))
  except ValueError, e:
    print e
  else:
    break

