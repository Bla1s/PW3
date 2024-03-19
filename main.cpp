#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

int process_file(const char *filename, const char *search_str)
{
    int occurrences = 0;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: File " << filename << " does not exist" << std::endl;
        return 0;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.find("\\input{") == 0)
        {
            // Cutting out the file name from the found line
            std::string included_filename = line.substr(7, line.find('}', 7) - 7);

            // Duplicating process
            pid_t pid = fork();

            if (pid == 0)
            {
                // Successfully created child process, so execute the following code
                occurrences = process_file(included_filename.c_str(), search_str);
                exit(occurrences);
            }
            else if (pid < 0)
            {
                // Error in duplicating child process
                std::cerr << "Error in duplicating child process." << std::endl;
            }
            else
            {
                // Waiting for the child process to finish
                int status;
                waitpid(pid, &status, 0);
                occurrences += WEXITSTATUS(status);
            }
        }
        else if (line.find(search_str) != std::string::npos)
        {
            std::cout << "Found \"" << search_str << "\" in file " << filename << std::endl;
            occurrences++;
        }
    }

    return occurrences;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Incorrect number of arguments. Please try again.\nUsage: ./a.out <filename> <search_str>" << std::endl;
        return 1;
    }

    const char *filename = argv[1];
    const char *search_str = argv[2];

    int total_occurrences = process_file(filename, search_str);
    std::cout << "Total occurrences of '" << search_str << "': " << total_occurrences << std::endl;

    return 0;
}
