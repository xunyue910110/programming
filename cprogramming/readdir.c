#include <stdio.h>

#include <dirent.h>

int main()
{
    DIR *dir = opendir("./");
    struct dirent *ent ;
    while (ent = readdir(dir))
    {
        printf("%d, %s\n", ent->d_type, ent->d_name);
    }
}
