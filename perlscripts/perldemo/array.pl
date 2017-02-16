@rocks = qw/ bedroom slate lava/;
print "@rocks\n";

foreach   ( 1 .. 10){
    print $_ * 2 . "\n";
}


$_ = "HELLO";
print;

@array = 5 .. 9;
foreach(@array)
{
    print ;
}

$fred = pop(@array);
print "@array";
print $fred;
push(@array, "10");
print("@array");

print "\n";
$_ = pop @array;
print  $_. "\n";

print "@array\n";
shift(@array);
print "@array\n";

@others = qw/9 0 2 1 0/;
push @array, @others;
print "@array\n";

@array = qw# dino  fred barney #;
print "@array\n";

$m = shift (@array);
print $m . "\n";
print "@array\n";

$n = shift @array;
$o = shift @array;
print "@array\n";
unshift @array, "5";
unshift @array, "4";
print "@array\n";

unshift @array, 1..3;
print "@array\n";


@fred = 6 ..10;
@barney = reverse @fred;
print "@barney\n";
print "@fred\n"
;

@wilma  = reverse 6.. 10;
print "@wilma\n";

@bill = 1..10;
print "@bill\n";
@bill = reverse @bill;
print "@bill\n";

@rocks = qw / bedrock slate rubble /;
@sorted = sort @rocks;
print "@sorted\n";

@back = reverse sort @rocks;
print "@back\n";

@rocks = sort @rocks;
print "@rocks\n";

@numbers = (20,1, 10, 9, 2);
print "@numbers\n";
@out = sort {$a<=>$b} @numbers;
print "@out\n";

@fred = 6 * 7;
@barney = "hello" . " " . "world";
@wilma = undef;

print "@fred\n";
print "@barney\n";
print "@wilma\n";

@betty = ();

@rocks = qw { talc quartz jade obsidian };
print "How many rocks do you have?\n";
print "I have ", @rocks, " rocks!\n";
print "I have ", scalar @rocks, " rocks!\n";

print "现在开始输入\n";
chomp( $lines = <STDIN>);
print "$lines\n";

chomp( @lines = <STDIN>);
print "@lines\n";
