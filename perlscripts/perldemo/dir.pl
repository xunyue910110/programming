use strict;
my $dir = "/home/developer/perldemo";

opendir DH, $dir or die "Cannot open $dir: $!";
foreach my $dirent (readdir DH)
{
    next if $dirent eq "." or $dirent eq "..";
    next if -d $dirent;
    next unless $dirent =~ /\.txt$/;

    my $fullname = "$dir/$dirent";
    print $fullname, "\n";
}
closedir DH;
