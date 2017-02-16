def g(x=0,y=0): return x + y
def usersum(*seq):
  seq = list(seq)
  s = seq.pop(0)
  for i in seq:
    s += i
  return s

def h(a,b,c):
  print a + b + c

def read(flike):
  if hasattr(flike, 'read'):
    return f.read()
  else:
    return None

if __name__ == '__main__':
  args = [1,2,3]
  kargs = dict(a=1, b=2, c = 3)
  print h(**kargs)
