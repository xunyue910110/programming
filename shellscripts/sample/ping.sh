for ip in 192.168.1.{1..255}
do
    ping $ip -c 2 &>/dev/null
    if [ $? -eq 0 ]
    then
      echo $ip is alive
#    else
#      echo $ip is die
    fi
done
