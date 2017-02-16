z = 10
def f (x,y,z):
  def helper(y):
    global z
    return x + y + z
  return helper(y)

x = 0
def f():
  #global x
  #x = 0
  f.x += 1
  return f.x
f.x = 0

if __name__ == '__main__':
  print f()
  print f()
