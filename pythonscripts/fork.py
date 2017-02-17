import os
import time

def myfork():
	pid = os.fork()
	if pid == 0:
		print("this is in child pid=%d parent pid = %d" % (os.getpid(), os.getppid()))
		
	else:
		status = os.waitpid(pid,0)
		print(status)
		print("this is in PARENT pid=%d" % os.getpid())
if __name__ == '__main__':
	myfork()
