use File::Basename;
use File::Spec;
use CGI;
use DBI;

my $name = "/home/developer/perldemo/1.txt";
(my $basename = $name) =~ s#^.*/##;
(my $dir = $name) =~ s#[^/]*$##;
print $basename . "\n";
print $dir . "\n";

#           ($name,$path,$suffix) = fileparse($fullname,@suffixlist);
#           $name = fileparse($fullname,@suffixlist);

       #    $basename = basename($fullname,@suffixlist);
       #    $dirname  = dirname($fullname);

$basename = basename $name;
$dirname = dirname $name;

print "$basename $dirname\n";

$dirname = $ENV{'HOME'};
$filename = 'a.txt';

my $new_name = File::Spec->catfile($dirname, $filename);
print "$new_name\n";
