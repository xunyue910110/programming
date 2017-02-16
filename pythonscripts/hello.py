#!/usr/bin/env python

import sys

if __name__ == '__main__':

  if len(sys.argv)-1 >= 1:
    name = sys.argv[1]
  else:
    name = 'Python'

  print 'Hello, %s!' % name
