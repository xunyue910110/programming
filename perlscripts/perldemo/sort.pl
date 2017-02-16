use strict;

my @rocks = qw / bedrock slate rubble granite /;
my @sorted = sort @rocks;
print "@sorted\n";

sub bynum {
    $a < $b ? -1 : $a == $b ? 0 : 1;
    #$a <=> $b;
}
my @numbers = (20, 1, 10, 2);
#my @sorted2 = sort {$a<=>$b} @numbers;
#my @sorted2 = sort bynum @numbers;
my @sorted2 = sort {$a < $b ? -1 : $a == $b ? 0 : 1;} @numbers;
print "@sorted2\n";


my @result = sort qw( cat Apple Big Cat Dog);
#my @result = sort {"\L$a" cmp "\L$b";} @result;
my @result = sort {lc "$a" cmp lc "$b";} @result;
print "@result\n";

my %elems = (B=>5, Be=>4, H=>1, He=>2, Li=>3);
my @result = sort  
{
    $elems{$b} <=> $elems{$a};
}
keys %elems;
print "@result\n";
