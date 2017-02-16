use strict;

#my @odds = grep $_ % 2 == 1, (1..100);
#print "@odds\n";

my @matching_lines = grep /fred/i, <>;
print "@matching_lines\n";
