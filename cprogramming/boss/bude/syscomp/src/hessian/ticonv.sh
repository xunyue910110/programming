$1 -c ticonv.cpp 1>/dev/null 2>&1
if [ "$?" = "0" ]; then
        echo "-DNEW_ICONV"
fi
