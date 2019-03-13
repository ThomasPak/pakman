#include <string>
#include <vector>

void vector_argv(const std::vector<std::string>& cmd_tokens,
                 std::vector<const char*>& argv)
{

    for (auto it = cmd_tokens.begin();
         it != cmd_tokens.end(); it++)
        argv.push_back(it->c_str());
    argv.push_back(NULL);
}
