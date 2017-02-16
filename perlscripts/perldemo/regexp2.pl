use strict;

$_ = "I thought you said Fred and <BOLD>Velma</BOLD>, not <BOLD>Wilma</BOLD>";
s#<BOLD>(.*?)</BOLD>#$1#g;
print $_;

$_ = "helloooooooooooooooooo";
if (/(hello??)/){
    print "$1\n";
}

$_ = "this is the first line\nthis is the second line\nthis is the third line\n";
s/^this/that/mg;
print "$_\n";


my $filename = "1.txt";
open FILE, $filename or die "can not open the file";
my $lines = join "", <FILE>;
$lines =~ s/^/$filename:/gm;
print "$lines";
