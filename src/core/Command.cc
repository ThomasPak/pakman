#include <string>

#include <string.h>

#include "utils.h"

#include "Command.h"

// Default constructor
Command::Command() : m_argv(nullptr)
{
}

// Construct from string
Command::Command(const std::string& raw_command)
    : m_raw_command(raw_command)
{
    // Split raw command into tokens
    m_cmd_tokens = parse_command(m_raw_command);

    // Allocate memory for argv
    m_argv = new char*[m_cmd_tokens.size() + 1];

    // Copy command tokens to argv
    copyCommandTokensToArgv();
}

// Construct from c-style string
Command::Command(const char raw_command[]) :
    Command(static_cast<std::string>(raw_command))
{
}

// Copy constructor
Command::Command(const Command& command) :
    m_raw_command(command.m_raw_command), m_cmd_tokens(command.m_cmd_tokens)
{
    // Allocate memory for argv
    m_argv = new char*[m_cmd_tokens.size() + 1];

    // Copy command tokens to argv
    copyCommandTokensToArgv();
}

// Move constructor
Command::Command(Command&& command) :
    m_raw_command(std::move(command.m_raw_command)),
    m_cmd_tokens(std::move(command.m_cmd_tokens))
{
    // Move argv
    m_argv = command.m_argv;
    command.m_argv = nullptr;
}

// Copy-assignment constructor
Command& Command::operator=(const Command& command)
{
    // Copy assign raw command
    m_raw_command = command.m_raw_command;

    // Free argv if argv is not nullptr
    if (m_argv != nullptr)
        freeArgv();

    // Copy assign command tokens
    m_cmd_tokens = command.m_cmd_tokens;

    // Allocate memory for argv
    m_argv = new char*[m_cmd_tokens.size() + 1];

    // Copy command tokens to argv
    copyCommandTokensToArgv();

    return *this;
}

// Move-assignment constructor
Command& Command::operator=(Command&& command)
{
    // Move assign raw command
    m_raw_command = std::move(command.m_raw_command);

    // Free argv if argv is not nullptr
    if (m_argv != nullptr)
        freeArgv();

    // Move assign command tokens
    m_cmd_tokens = std::move(command.m_cmd_tokens);

    // Move argv
    m_argv = command.m_argv;
    command.m_argv = nullptr;

    return *this;
}

// Destructor
Command::~Command()
{
    // Free argv if argv is not nullptr
    if (m_argv != nullptr)
        freeArgv();
}

// Return command as string
const std::string& Command::str() const
{
    return m_raw_command;
}

// Return argv
char** Command::argv() const
{
    return m_argv;
}

// Copy command tokens to argv
void Command::copyCommandTokensToArgv()
{
    // Loop over command tokens and copy to argv
    for (int i = 0; i < m_cmd_tokens.size(); i++)
    {
        // Allocate memory for argv token
        m_argv[i] = new char[m_cmd_tokens[i].size() + 1];

        // Copy to argv
        strcpy(m_argv[i], m_cmd_tokens[i].c_str());
    }

    // Last entry is null pointer
    m_argv[m_cmd_tokens.size()] = nullptr;
}

// Free argv
void Command::freeArgv()
{
    // Loop over argv
    for (int i = 0; i < m_cmd_tokens.size(); i++)
        // Free argv token
        delete[] m_argv[i];

    // Free argv and assign nullptr
    delete[] m_argv;
    m_argv = nullptr;
}

