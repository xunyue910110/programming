my $date = localtime;

$^I = "*.bak";
while (<>){
s/^888/99999/;
s/^123\n//;
s/^Date:.*/Date: $date/;
print ;
}
