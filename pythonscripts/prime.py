s = range(100)
for i in s:
  if i != 0 and i != 1:
    print i
    for j in s[i*2::i]:
      s[j] = 0

