
use strict;

my $line ;
chomp ($line=<STDIN>);
print "$line"."\n";

#undef
#end of file
while (<STDIN>)
{
    print "I saw $_";
}
foreach(<STDIN>)
{
    print "I saw $_";
}

while (<>)
{
    chomp ;
    print "It was $_ that I saw.\n";
}
