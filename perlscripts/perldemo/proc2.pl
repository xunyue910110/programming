defined(my $pid = fork) or die "Cannot fork: $!";
unless ($pid){
    exec "date";
    die "Cannot exec date: $!";
}

waitpid($pid, 0);


