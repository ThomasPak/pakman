#include <sstream>
#include <iostream>
#include <random>

#include <assert.h>

#include "serialisation.h"
#include "deserialisation.h"

int main()
{
    std::ostringstream osstr;
    std::istringstream isstr;

    // Test serialising LineString
    {
        osstr.str("");
        std::string key("my_line_string");
        LineString val = "this_is_a_string\n\n\n";

        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == "my_line_string:this_is_a_string\n");
    }

    // Test deserialising LineString
    {
        isstr.str("my_line_string:this_is_a_string\n");

        auto val = deserialise_scalar_value<LineString>("my_line_string", isstr);
        assert(val.str() == "this_is_a_string");
    }

    // Test serialising double
    {
        osstr.str("");
        std::string key("my_double");
        double val = 0.128;

        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == "my_double:0.128\n");
    }

    // Test deserialising double
    {
        isstr.str("my_double:0.128\n");

        auto val = deserialise_scalar_value<double>("my_double", isstr);
        assert(val == 0.128);
    }

    // Test serialising int
    {
        osstr.str("");
        std::string key("my_int");
        int val = 420;

        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == "my_int:420\n");
    }

    // Test deserialising int
    {
        isstr.str("my_int:420\n");

        auto val = deserialise_scalar_value<int>("my_int", isstr);
        assert(val == 420);
    }

    // Test serialising bool
    {
        osstr.str("");
        std::string key("my_bool");
        bool val = true;

        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == "my_bool:true\n");
    }

    // Test deserialising bool
    {
        isstr.str("my_bool:true\n");

        auto val = deserialise_scalar_value<bool>("my_bool", isstr);
        assert(val == true);
    }

    // Test serialising string
    {
        osstr.str("");
        std::string key("my_string");
        std::string val = "this is\na string\nboo\n\n";

        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == "my_string:22:this is\na string\nboo\n\n\n");
    }

    // Test deserialising string
    {
        isstr.str( "my_string:22:this is\na string\nboo\n\n\n");

        auto val = deserialise_scalar_value<std::string>("my_string", isstr);
        assert(val == "this is\na string\nboo\n\n");
    }

    // Test deserialising string, string size exception
    try
    {
        isstr.str( "my_string:a:this is\na string\nboo\n\n\n");

        auto val = deserialise_scalar_value<std::string>("my_string", isstr);

        throw; // Do not reach this line
    }
    catch (const std::exception& e)
    {
        assert(dynamic_cast<const std::runtime_error*>(&e) != nullptr);

        assert(std::string(e.what()) == "deserialisation failed, "
                "could not extract string size");
        isstr.clear();
    }

    // Test deserialising string, extract string exception
    try
    {
        isstr.str( "my_string:50:this is\na string\nboo\n\n\n");

        auto val = deserialise_scalar_value<std::string>("my_string", isstr);

        throw; // Do not reach this line
    }
    catch (const std::exception& e)
    {
        assert(dynamic_cast<const std::runtime_error*>(&e) != nullptr);

        assert(std::string(e.what()) == "deserialisation failed, "
                "could not extract string");
        isstr.clear();
    }

    // Test serialising Command
    {
        osstr.str("");
        std::string key("my_command");
        Command val = "fake_command \"haha\n\"";

        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == "my_command:20:fake_command \"haha\n\"\n");
    }

    // Test deserialising Command
    {
        isstr.str("my_command:20:fake_command \"haha\n\"\n");

        auto val = deserialise_scalar_value<Command>("my_command", isstr);
        assert(val.str() == "fake_command \"haha\n\"");
    }

    // Test serialising pending TaskHandler
    {
        osstr.str("");
        std::string key("my_task_handler");
        TaskHandler val("my input\nstring");

        std::ostringstream int_sstr;
        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == "my_task_handler:m_input_string:15:my input\nstring\n"
                "my_task_handler:m_output_string:0:\n"
                "my_task_handler:m_error_code:-1\n");
    }

    // Test deserialising pending TaskHandler
    {
        isstr.str("my_task_handler:m_input_string:15:my input\nstring\n"
                "my_task_handler:m_output_string:0:\n"
                "my_task_handler:m_error_code:-1\n");

        auto val = deserialise_scalar_value<TaskHandler>("my_task_handler", isstr);
        assert(val.isPending());
        assert(val.getInputString() == "my input\nstring");
        assert(val.getOutputString() == "");
        assert(val.getErrorCode() == -1);
    }

    // Test serialising finished TaskHandler
    {
        osstr.str("");
        std::string key("my_task_handler");
        TaskHandler val("my input\nstring");
        val.recordOutputAndErrorCode("my\noutput\nstring", 5);

        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == "my_task_handler:m_input_string:15:my input\nstring\n"
                "my_task_handler:m_output_string:16:my\noutput\nstring\n"
                "my_task_handler:m_error_code:5\n");
    }

    // Test deserialising finished TaskHandler
    {
        isstr.str("my_task_handler:m_input_string:15:my input\nstring\n"
                "my_task_handler:m_output_string:16:my\noutput\nstring\n"
                "my_task_handler:m_error_code:5\n");

        auto val = deserialise_scalar_value<TaskHandler>("my_task_handler", isstr);
        assert(val.isFinished());
        assert(val.getInputString() == "my input\nstring");
        assert(val.getOutputString() == "my\noutput\nstring");
        assert(val.getErrorCode() == 5);
    }

    // Test serialising 64-bit Mersenne Twister pseudo random number generator
    {
        osstr.str("");
        std::string key("my_prng");
        std::mt19937_64 val;

        std::ostringstream prng_sstr;
        prng_sstr << val;

        serialise_scalar_value(key, val, osstr);
        assert(osstr.str() == ("my_prng:" + prng_sstr.str() + "\n"));
    }

    // Test deserialising 64-bit Mersenne Twister pseudo random number generator
    {
        std::mt19937_64 myval;
        std::stringstream sstr;
        sstr << myval;

        isstr.str("my_prng:" + sstr.str() + "\n");

        auto val = deserialise_scalar_value<std::mt19937_64>("my_prng", isstr);
        assert(val == myval);
    }

    // Test invalid key exception
    try
    {
        isstr.str("my_line_string:this_is_a_string\n");

        auto val = deserialise_scalar_value<LineString>("fake_key", isstr);

        throw; // Do not reach this line
    }
    catch (const std::exception& e)
    {
        assert(dynamic_cast<const std::runtime_error*>(&e) != nullptr);

        assert(std::string(e.what()) == "deserialisation failed, "
                "read key ('my_line_string') did not match"
                " queried key ('fake_key')");
        isstr.clear();
    }

    // Test invalid format exception
    try
    {
        isstr.str("my_line_string this_is_a_string\n");

        auto val = deserialise_scalar_value<LineString>("my_line_string", isstr);

        throw; // Do not reach this line
    }
    catch (const std::exception& e)
    {
        assert(dynamic_cast<const std::runtime_error*>(&e) != nullptr);

        assert(std::string(e.what()) == "deserialisation failed, "
                "read key ('my_line_string this_is_a_string\n') did not match"
                " queried key ('my_line_string')");
        isstr.clear();
    }

    // Test empty value string exception
    try
    {
        isstr.str("my_line_string:\n");

        auto val = deserialise_scalar_value<LineString>("my_line_string", isstr);

        throw; // Do not reach this line
    }
    catch (const std::exception& e)
    {
        assert(dynamic_cast<const std::runtime_error*>(&e) != nullptr);

        assert(std::string(e.what()) == "deserialisation failed, "
                "could not extract value");
        isstr.clear();
    }
}
