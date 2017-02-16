use DBI;

@drivers = DBI->available_drivers;
print "@drivers\n"
