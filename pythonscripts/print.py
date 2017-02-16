def  name():
  print 'name'

def getint():
  while 1:
    try:
      i = int(raw_input('? '))
    except ValueError, e:
      print e
    else:
      break
  return i

if __name__ == '__main__':
  print getint()
  print getint()

