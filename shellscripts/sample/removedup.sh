cd $1
ls |grep -v f1 |grep -v f2 > f1
ls |grep -v f1 |grep -v f2|xargs md5sum |sort |uniq -w 32|awk '{print $2}'| sort > f2

comm f1 f2 -2 -3 > willrem
cat willrem|xargs rm

