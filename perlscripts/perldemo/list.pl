@fred[0] = "yabba";
@fred[1] = 'dabba';
@fred[2] = 'doo';
print @fred;

@fr = ("yabba", 'dabba', 'doo');
$fr[0] = "good";
print "\n";
print @fr[0];
print "\n";
print "@fr" ;
print "\n";

@grades = (99, 76, 100, 65, 68);
print "\n@grades\n";

$" = ",";
print "\n@grades\n";

$" = "|";
print "\n@grades\n";

$fred[111] = "dad";
print "\n@fred\n";
print $fred[$#fred];
