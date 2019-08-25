#ifndef OUTPUTSTREAMHANDLER_H
#define OUTPUTSTREAMHANDLER_H

#include <iostream>
#include <string>

/** A singleton class providing access to the Pakman output stream.
 *
 * OutputStreamHandler is a class that returns an instance to the Pakman output
 * stream.  By default, the output stream is the standard output.  However,
 * this default behaviour can be overriden by specifying an output file using
 * the command-line option `--output-file`.
 */

class OutputStreamHandler
{
    public:

        /** @return singleton instance. */
        static OutputStreamHandler* instance();

        /** Close file if a filename was given. */
        static void destroy();

        /** @return reference to output stream.*/
        std::ostream& getOutputStream();

    private:

        // Private default constructor
        OutputStreamHandler(const std::string& filename);

        // Private destructor
        ~OutputStreamHandler();

        // Output stream
        std::ostream *m_p_output_stream = &std::cout;

        // Filename
        std::string m_filename;

        // Static instance
        static OutputStreamHandler* s_instance;
};

#endif // OUTPUTSTREAMHANDLER_H
