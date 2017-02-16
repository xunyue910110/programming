#system ("date");

#my @whos = `who`;
#chomp @whos;
#print "@whos\n";
foreach (`who`){
    my ($user, $tty, $time) = /(\S+)\s+(\S+)\s+(.*)/;
    $ttys {$user} .= " $tty at $time";
}
while(my($k,$v) = each (%ttys))
{
    print "$k => $v\n";
}

my @whos = `who`;
print @whos;

open DATE, "date|" or die "cannot pipe from date:$!";
open WC, "|wc -w" or die "Can not pipe to wc:$!";
my $now = <DATE>;

print $now;
print WC $now ;

defined(my $pid = fork) or die "Cannot fork: $!";
unless ($pid){
    exec "date";
    die "Cannot exec date: $!";
}

waitpid($pid, 0);
