use strict;
sub calc{
    $a = $fred + $barney;
    $b = $fred - $barney;
    $c = $fred * $barney;
    $d = $fred / $barney;
}

$fred = 30;
$barney = 3;
$result = &calc;

print "$result\n";

sub larger {
    if ($fred > $barney){
    return $fred;
    }
    else{
    return   $barney;
    }
}
$result = &larger;
print "$result\n";

sub max {
   return ($_[0] > $_[1]) ? $_[0] : $_[1];
}

$result = &max(10, 25);
print "$result\n";

sub max {
    my ($m,$n ) = @_;
    return $m > $n ? $m : $n;
}
$result = &max(10, 25);
print "$result\n";

sub max{
    my $max = shift @_;
    foreach (@_)
    {
        if ( $max < $_){ $max = $_;}
    }
    return $max;
}


$result = &max(10, 25,9, 8, 7, 22);
print "$result\n";

sub demo {
    my($num1) = @_;
    my $num2 = @_;

    print "$num1\n";
    print "$num2\n";
}

&demo(8,9,10);
