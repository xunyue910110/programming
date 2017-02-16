

#$x undef
print $x + 1;

print $x * 1;


print $x . hello;

$k = 12;
print "\n";
$k = undef;
print $k;


$n = 1;
while ($n <= 100)
{
    $sum += $n;
    $n++;
}

print $sum;

$madonna = <STDIN>;
if (defined($madonna)){
    print "The input was $madonna";
}
else {
    print "No data available.\n";
}
