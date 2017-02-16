for ($i = 0 ; $i < 6; $i++)
{
    print "entering $i\n";
    print "$i\n";
    print "$i\n";
    #last if ($i == 3);  #提前结束整个循环
    #next if  ($i == 3);  #进入下一循环
    #redo if ($i == 3);   #重做本次循环
    print "$i\n";
    print "$i\n";
}

print "循环全部结束\n";

