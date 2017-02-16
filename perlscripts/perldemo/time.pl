print localtime() . "\n";
print gmtime() . "\n";

print localtime;
print "\n";
my $now = localtime;
print "$now\n";
print scalar localtime;
print "\n";

my ($sec, $min, $hour, $day, $mon, $year, $wday, $yday) = localtime;

print $sec . "\n";
print $min . "\n";
print  $hour . "\n";
print  $day . "\n";
print 1+ $mon . "\n";
print 1900+ $year."\n";
print $wday, "\n";
print $yday, "\n";

my $time = time;
print scalar localtime $time;
