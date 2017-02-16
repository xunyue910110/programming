use strict;

while ( <> ){
    chomp;
    my ($card_num, $count) = (split /:/)[0,4];
    #my ($card_num, $count) = (split /:/)[0,4];
    #my ($card_num, $count) = ($items[0], $items[4]);
    print "$card_num\t $count\n";
}

my $devno = (stat("1.txt")) [1];
print $devno;

my %scores = ("barney"=>195, "fred" =>205);
my @two = @scores{ qw /barney fred/ };
print "@two \n";
print "\n";

@scores{ qw /baney fred/ } = (85, 20);
while (my ($k, $v) = each (%scores)) {
    print "$k => $v\n";
}

