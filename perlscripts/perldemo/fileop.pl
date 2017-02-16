use strict;

#my @files = glob "*.txt *.pl";
my @files = <*.txt *.pl>;
foreach (@files)
{
    print $_ . "\n";
}

my $success = unlink glob "[23].txt";
print $success . "\n";

foreach my $file( glob "[23].txt")
{
    unlink $file or warn "failed on $file: $!\n";

}

my $now = time;
my $yest = $now - (24 * 60 *60);
print $now . "\n";
print $yest . "\n";
utime $now, $yest, glob "*.txt";
