#!/usr/bin/perl

$a = "hello\n\n";
chomp $a;
$n = chomp $a;
$n = chomp $a;
print $a, $n;

print "pls input your name: ";
$name = <STDIN>;
chomp ($name);
print "your name is :", $name;
