import sys,os
def daemon_test():
	pid  = os.fork()
	if pid >0: 
		sys.exit(0)
	os.setsid()
	os.umask(0)
	pid = os.fork()
	if pid >0:
		sys.exit(0)

	with open(r'text.txt', 'w') as f:
		a = 1
		while True:
			a = a+1
			f.write(str(a))

daemon_test()
