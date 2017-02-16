use strict;

my %score;
$score{"张飞"} = 99;
$score{"刘备"} = 76;
$score{"关雨"} = 59;

print $score{"刘备"};
print "\n";

foreach (qw(张飞 刘备 关雨))
{
    print $_ . ":" . $score{$_} . "\n";
}

print $score{'林冲'};
print "\n";

my %some_hash = ("foo"=>25, "bar"=>12.4, 2.5=>"good");
print $some_hash{"2.5"};
print "\n";

my @array = %some_hash;
print "@array\n";

my %new_hash = %some_hash;
print %new_hash;
print "\n";

my %a = ("one" => 1, "two" =>2, "three" => 3);
my %b = %a;
%b = reverse(%a);
print %b;
print "\n";

my %hash = ("a"=>1, "b"=>2, "c"=>3);
my @k = keys %hash;
my @v = values %hash;
print "@k\n";
print "@v\n";

my $count = @k;
print $count . "\n";

while (my ($key, $value) = each %hash){
    print "$key => $value\n";
}

foreach my $key (sort keys %hash){
   print "$key => $hash{$key}\n"; 
}

if ( defined($hash{"d"})){
    print "have ";
}
else{
    print "not found.\n";
}

delete $hash{"b"};
foreach my $key (sort keys %ENV){
   print "$key => $ENV{$key}\n"; 
}


