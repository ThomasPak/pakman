#include <string>
#include <vector>

std::vector<const char*> vector_argv(
        const std::vector<std::string>& cmd_tokens)
{
    // Initialize
    std::vector<const char*> argv;

    // Convert to vector of c-style string
    for (const std::string& it : cmd_tokens)
        argv.push_back(it.c_str());

    // Append nullptr
    argv.push_back(nullptr);

    return argv;
}
