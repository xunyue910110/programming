use strict;

sub is_leap_year {
    my $year = shift @_;
    return (($year % 4 == 0 ) && ($year % 100  != 0) || ($year % 400 == 0));
}

if (is_leap_year(@ARGV[0]))
{
    print "Leap year\n";
}
else
{
    print "Not leap year\n";
}

foreach(2000..2200)
{
    if (is_leap_year($_)){
        print "$_ is leap year.\n";
    }
    else{
        print "$_ is not leap year.\n";
    }
}
