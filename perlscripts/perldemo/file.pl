use strict;

my $filename = "1.txt";
print "File is exist\n" if -e $filename;
print "File is readable\n" if -r $filename;
print "File is writable\n" if -w $filename;
print "File is executable\n" if -x $filename;
print "File is a regular file\n" if -f $filename;
print "File is a directory\n" if -d $filename;
print "File size is ",   -s $filename , "\n";


my @l = stat($filename);
print "size: ", $l[7];
print "@l\n";

my $file = "1.txt";
print "file is readble and writable\n" if ( -r $file and -w _);
print "file is readble\n" if ( -r $file );
print "file is writable \n" if -w _;

print -M  "../src.3e.tar.gz" > 10 and 
      -s "../src.3e.tar.gz" > 100_000 and
        -A "../src.3e.tar.gz" > 10 ;

