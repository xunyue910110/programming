#!/usr/bin/env python
# -*- coding: utf-8 -*-

weight = 64
height = 163.0
bmi = weight / (height/100) ** 2

print '%.2f' % bmi,

thresholds = [18.5, 24, 27, 30, 35]

levels = ['体重过轻', '正常', '过重', '轻度肥胖', '中度肥胖', '重度肥胖']

for threshold, level in zip(thresholds, levels):
  if bmi < threshold:
    print level
    break
else:
  print levels[-1]
