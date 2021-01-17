#ifndef LINESTRING_H
#define LINESTRING_H

#include <string>

/** A class for representing single line strings.
 *
 * LineString is a class that takes a string with any number of trailing
 * newlines, and removes them.  If there are any newlines left in the string
 * after this operation, an exception is thrown.  Therefore, the only valid
 * strings to construct from are those that have no newlines or newlines only
 * at the end of the string.
 *
 * The resulting string is accessed by the function str().
 */

class LineString
{
    friend std::ostream& operator<<(std::ostream& out,
            const LineString& line_string);
    friend std::istream& operator<<(std::istream& in,
            LineString& line_string);

    public:

        /** Default constructor does nothing. */
        LineString() = default;

        /** Construct from lvalue string.
         *
         * @param raw_string  raw lvalue string.
         */
        LineString(const std::string& raw_string);

        /** Construct from rvalue string.
         *
         * @param raw_string  raw rvalue string.
         */
        LineString(std::string&& raw_string);

        /** Construct from C-style string.
         *
         * @param raw_string  C-style string.
         */
        LineString(const char raw_string[]);

        /** Default copy constructor.
         *
         * @param linestring  source LineString object.
         */
        LineString(const LineString& linestring) = default;

        /** Default move constructor.
         *
         * @param linestring  source LineString object.
         */
        LineString(LineString&& linestring) = default;

        /** Default copy-assignment constructor.
         *
         * @param linestring  source LineString object.
         *
         * @return reference to copy-assigned LineString object.
         */
        LineString& operator=(const LineString& linestring) = default;

        /** Default move-assignment constructor.
         *
         * @param linestring  source LineString object.
         *
         * @return reference to move-assigned LineString object.
         */
        LineString& operator=(LineString&& linestring) = default;

        /** Default destructor does nothing. */
        ~LineString() = default;

        /** @return string without newlines. */
        const std::string& str() const;

        /** @return size of string without newlines. */
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
