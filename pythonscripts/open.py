f = open("sqlok") 
#print(f.read())
	

f2 = open("dataok")
#print(f2.read())

f3 = open("resultok", "w")

line2 = f2.readline()
print(line2)
for line in f:
	print(line.rstrip())
	l = line.split()
	print(l)
	newline = line2.replace('$1',l[0]).replace('$2', l[1])
	f3.write(newline)

f.close()
f2.close()

f3.close()

	
