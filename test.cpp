#include <iostream>
#include <vector>
#include <dirent.h>

int main()
{
    DIR *dir; struct dirent *diread;
    std::vector<std::string> files;

    if ((dir = opendir(".")) != nullptr)
    {
        while ((diread = readdir(dir)) != nullptr)
        {
            files.push_back(diread->d_name);
        }
        closedir (dir);
    }
    else
    {
        perror ("opendir");
        return EXIT_FAILURE;
    }

    for (std::vector<std::string>::iterator _it = files.begin(); _it != files.end(); _it++)
        std::cout << *_it << " | ";
    std::cout << std::endl;

    return EXIT_SUCCESS;
}