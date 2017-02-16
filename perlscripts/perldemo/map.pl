use strict;

my @files = glob "*.*";
print "@files\n";

my @txt_files = map /(.*\.txt$)/, @files;
print "@txt_files\n";
my @txt_files = grep /(.*\.txt$)/, @files;
print "@txt_files\n";

my @sizes = map -s, @txt_files;
print "@sizes\n";

sub big_money {
    my $number = sprintf "%.2f", shift @_;
    1  while $number =~ s/^(-?\d+)(\d\d\d)/$1,$2/    ;
    $number =~ s/^(-?)/$1\$/;
    $number;
}

print big_money 1234566.9393 ;
print "\n";

my @data = (4.75, 1.5, 2, 1234, 6.9456, 12345678.9, 29.95);
#my @res = map big_money($_), @data;
print "The money numbers are:\n", map {sprintf"%24s\n", &big_money($_) } @data;

print "Some powers of two are:\n", map "\t" . (2 ** $_) . "\n", (0..15);
