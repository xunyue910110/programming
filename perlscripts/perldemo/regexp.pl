use encoding 'utf8';
use strict;
$_ = "我正在学习数据结构与算法";

#.不包括\n
if (/数据结构.算法/) {
    print "找到了\n";
}else{
    print "没找到\n";
}

if (/学习.?算法/){print "找到了\n"; } else{
print "没找到\n";}


$_ = "Welcome to our website http://www.liebao.net.cn";
if (m\http://\) {print "Found\n";} else {print "Not Found.\n"; }
if (m{http://}) {print "Found\n";} else {print "Not Found.\n"; }

print "Would you like to play a game?";
print "\n";

chomp ($_=<STDIN>);
if (m{yes}i) {print "Found\n";} else {print "Not Found.\n"; }

$_ = "he's out bowling with Barney tonight.";
s/Barney/Fred/;
print "$_\n";

s/with (\w+)/against $1's team/;
print "$_\n";

$_ = "one two three";
#s/(\w+) (\w+)/$2, $1/;
print "$_\n";

s/^/huge,/;
print "$_\n";

$_ = "home, sweet home";
s/home/cave/;
print "$_\n";

$_ = "Input data\t may have  extra whitespace.";
s/\s+/ /g;
print "$_\n";

$_ = "        one two        ";
s/^\s+|\s+$//g;
print "$_ sss\n";
s/^\s+//;
print;
print "\n";

s/\s+$//;
print "$_\n";

s#^https://#http://#;

$_ = "Fred fintstone ";
if (s/fred/wilma/)
{
    print "succesfully\n";
}

my $content = "one another one";
$content =~ s/(one)/\U$1/g;
$content =~ s/(ONE)/\L$1/g;
print "$content\n";

$_ = "this is one and two";
s/(\w+) and (\w+)/\U$2\E and \U$1/;
print "$_\n";

my @fields = split /:/, "abc:def::g:h:::::::::";
print "$#fields\n";

my $some_input = "This is a \t test.\n";
$_ = "This is a \t test.\n";
my @args = split /\s+/;
print "$#args\n";

my $y = join "-",  @args;
print "$y\n";

$_ = "Hello there, neighbor!";
my ($first, $second, $third) = /(\S+) (\S+), (\S+)/;
print "$first\n";
print "$second\n";
print "$third\n";

my $text = "Fred dropped a 6 ton granite block on Mr.Slate";
my @worlds = ($text =~ /([a-z]+)/ig);
print "@worlds\n";

my $data  = "Barney Rubble Fred Flintstone Wilma Flintstone Bill Gates";
my %last = ($data =~ /(\w+)\s+(\w+)/g);
while ( my ($key, $value ) = each %last){
    print $key, $value;
    print "\n";
}
