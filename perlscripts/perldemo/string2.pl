use strict;

my $stuff = "Howdy world!";
my $where = index $stuff, "wor";
print $where . "\n";

my $where2 = index $stuff, "w";
print $where2 . "\n";

my $where3 = index $stuff, "w", $where2 + 1;
print $where3 . "\n";

my $where4 = index $stuff, "w", $where3+1;
print $where4 . "\n";

my $last_slash = rindex ("/etc/passwd", "/", );
print $last_slash . "\n";

my $name = "abc/123/xyz/hello";
my $where1 = rindex($name, "/");
print $where1 . "\n";

$where2 = rindex($name, "/", $where1 -1);
print $where2 . "\n";

$where2 = rindex($name, "/", $where2 -1);
print $where2 . "\n";

my $a = substr("Fred J. Flintstone", 8, 5);
print $a . "\n";

my $b = substr "Fred J. Flintstone", 13, 1000;
print $b . "\n";
my $b = substr "Fred J. Flintstone", 13;
print $b . "\n";

my $out = substr "some very long string", -3, 2;
print "$out\n";

my $string = "hello, world!";
substr($string, 0, 5) = "Goodbye";
print $string . "\n";

$string = "aaaaaaaaabfredbbbbbbbbbbbbccccfred...";
substr($string, index($string, "fred")) =~ s/fred/barney/g;
print $string . "\n";
$string = "aaaaaaaaabfredbbbbbbbbbbbbccccfred...";
$string =~ s/fred/barney/g;
print $string . "\n";

$string = "hello world";
substr($string, 0, 5, "goodbye");
print $string. "\n";
#print $pre  . "\n";

my $now = localtime;
print $now . "\n";

my $mon = 2414_223_456_789.249274;
sub big_money {
    my $number = sprintf("%.2f", shift @_);
    while ($number =~ s/^(-?\d+)(\d\d\d)/$1,$2/)
     {
      ;  
    }
    $number =~ s/^(-?)/$1\$/;
    $number; 
}

print &big_money($mon);

