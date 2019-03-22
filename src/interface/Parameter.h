#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>

class Parameter
{
    public:

        // Default constructor
        Parameter() = default;

        // Construct from lvalue string
        Parameter(const std::string& raw_parameter);

        // Construct from rvalue string
        Parameter(std::string&& raw_parameter);

        // Construct from c-style string
        Parameter(const char raw_parameter[]);

        // Default copy, move, copy-assignment, move-assignment and destructor
        Parameter(const Parameter&)             = default;
        Parameter(Parameter&&)                  = default;
        Parameter& operator=(const Parameter&)  = default;
        Parameter& operator=(Parameter&&)       = default;
        ~Parameter()                            = default;

        // Return string
        const std::string& str() const;

        // Return size
        size_t size() const;

    private:

        // Remove trailing newlines
        void removeTrailingNewlines();

        // Check for newlines
        void checkForNewlines() const;

        // Save parameter as string
        std::string m_parameter;
};

#endif // PARAMETER_H
