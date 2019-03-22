#ifndef LINESTRING_H
#define LINESTRING_H

#include <string>

class LineString
{
    public:

        // Default constructor
        LineString() = default;

        // Construct from lvalue string
        LineString(const std::string& raw_string);

        // Construct from rvalue string
        LineString(std::string&& raw_string);

        // Construct from c-style string
        LineString(const char raw_string[]);

        // Default copy, move, copy-assignment, move-assignment and destructor
        LineString(const LineString&)               = default;
        LineString(LineString&&)                    = default;
        LineString& operator=(const LineString&)    = default;
        LineString& operator=(LineString&&)         = default;
        ~LineString()                               = default;

        // Return string
        const std::string& str() const;

        // Return size
        size_t size() const;

    private:

        // Remove trailing newlines
        void removeTrailingNewlines();

        // Check for newlines
        void checkForNewlines() const;

        // Save as string
        std::string m_line_string;
};

#endif // LINESTRING_H
