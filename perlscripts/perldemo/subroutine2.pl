use strict;
use 5.010;


my @names = qw / fred barney betty dino wilma pebbles bann /;
my $result = &find ("dino", @names);

if ( $result != -1)
{
    print $names[$result];
}
else 
{
    print "Not found.\n";
}

sub find {
    my($what , @array) = @_;
    foreach( 0..$#array){
        if ($what eq $array[$_])
        {
            return $_;
        }
    }
    return -1;    
}

sub get_list {
    my ($m, $n) = @_;
    if ( $m > $n) {return  $n .. $m;}
    else {return $m .. $n;}
}

my @arr = &get_list(30, 20);
print "@arr\n";

sub marine {
    state $n = 0;
    $n += 1;
    print "Hello ,sailor number $n!\n";
}
&marine;
&marine;
&marine;
&marine;


sub running_sum {
    state $sum = 0;
    state @numbers;
   # state @array = qw(a b c);
    foreach  (@_)
    {   
        push @numbers, $_;
        $sum += $_;
    }
    say "The sum of (@numbers) is $sum ";
}

&running_sum(5,6);
&running_sum(1..10);
