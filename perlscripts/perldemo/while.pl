$count = 0;

while ($count < 10)
{
   print $count; 
   $count += 2; 
}

for ($count=0; $count<10; $count++)
{
    print $count;
}

$count = 0;
do
{
print $count . "\n";
}while (++$count <10);


