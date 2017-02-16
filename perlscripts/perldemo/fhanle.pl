
open MYFILE, "<", "1.txt";
while (<MYFILE>){
    print $_;
}
close MYFILE;

open FILE, ">>", "b.txt";
print FILE "Hello Perl!\n";

print STDOUT "YES";
close FILE;

my $success = open FILEB, "<b.txt";
if (!$success ){
    print "打开文件失败\n";
}

(open FILEB, "<b.txt") ||
    die "Cannot open file: $!";

