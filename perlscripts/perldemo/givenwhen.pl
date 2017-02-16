use strict;
use 5.010;

my $name = $ARGV[0];

given ($name){
    when (/fred/i) { say 'Name has fred in it.';continue}
    when (/^Fred/ ){say 'Name starts with Fred.';continue}
    when ('Fred'){ say 'Name is Fred'; }
    default {say 'I do\'t see a Fred.';}
}

given($ARGV[1]){
when (/^-?\d+\.\d+$/) { say 'This is a number';continue;}
when ($_ > 10) { say "bigger than 10";}
when ($_ < 10 ) { say "Number is less than 10";}default { say "equal 10";} 
}
