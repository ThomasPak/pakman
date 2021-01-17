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

    // Test serialising vector of LineStrings
    {
        osstr.str("");

        std::string key("my_line_string_vector");

        std::vector<LineString> vals;
        vals.push_back("this_is_string_one\n\n\n");
        vals.push_back("this_is_string_two\n\n");
        vals.push_back("this_is_string_three");

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_line_string_vector:3\n"
                "my_line_string_vector_0:this_is_string_one\n"
                "my_line_string_vector_1:this_is_string_two\n"
                "my_line_string_vector_2:this_is_string_three\n");
    }

    // Test serialising vector of doubles
    {
        osstr.str("");

        std::string key("my_double_vector");

        std::vector<double> vals;
        vals.push_back(3.14);
        vals.push_back(0.128);
        vals.push_back(1e18);

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_double_vector:3\n"
                "my_double_vector_0:3.14\n"
                "my_double_vector_1:0.128\n"
                "my_double_vector_2:1e+18\n");
    }

    // Test serialising vector of ints
    {
        osstr.str("");

        std::string key("my_int_vector");

        std::vector<int> vals;
        vals.push_back(314);
        vals.push_back(128);
        vals.push_back(-44);

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_int_vector:3\n"
                "my_int_vector_0:314\n"
                "my_int_vector_1:128\n"
                "my_int_vector_2:-44\n");
    }

    // Test serialising vector of bools
    {
        osstr.str("");

        std::string key("my_bool_vector");

        std::vector<bool> vals;
        vals.push_back(true);
        vals.push_back(false);
        vals.push_back(false);

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_bool_vector:3\n"
                "my_bool_vector_0:true\n"
                "my_bool_vector_1:false\n"
                "my_bool_vector_2:false\n");
    }

    // Test serialising vector of strings
    {
        osstr.str("");

        std::string key("my_string_vector");

        std::vector<std::string> vals;
        vals.push_back("this is\na string\n\tlol");
        vals.push_back("\nthis is string two\n");
        vals.push_back("this is string three");

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_string_vector:3\n"
                "my_string_vector_0:21:this is\na string\n\tlol\n"
                "my_string_vector_1:20:\nthis is string two\n\n"
                "my_string_vector_2:20:this is string three\n");
    }

    // Test serialising vector of Commands
    {
        osstr.str("");

        std::string key("my_command_vector");

        std::vector<Command> vals;
        vals.push_back("fake_command \"one\n\"");
        vals.push_back("fake_command \"\ntwo\"");
        vals.push_back("fake_command \"three\n\"");

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_command_vector:3\n"
                "my_command_vector_0:19:fake_command \"one\n\"\n"
                "my_command_vector_1:19:fake_command \"\ntwo\"\n"
                "my_command_vector_2:21:fake_command \"three\n\"\n");
    }

    // Test serialising vector of pending TaskHandlers
    {
        osstr.str("");

        std::string key("my_task_vector");

        std::vector<TaskHandler> vals;
        vals.emplace_back("input string\none");
        vals.emplace_back("input string\ntwo");

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_task_vector:2\n"
                "my_task_vector_0:m_input_string:16:input string\none\n"
                "my_task_vector_0:m_output_string:0:\n"
                "my_task_vector_0:m_error_code:-1\n"
                "my_task_vector_1:m_input_string:16:input string\ntwo\n"
                "my_task_vector_1:m_output_string:0:\n"
                "my_task_vector_1:m_error_code:-1\n");
    }

    // Test serialising vector of finished TaskHandlers
    {
        osstr.str("");

        std::string key("my_task_vector");

        std::vector<TaskHandler> vals;

        vals.emplace_back("input string\none");
        vals.emplace_back("input string\ntwo");

        vals[0].recordOutputAndErrorCode("output\nstring one", 4);
        vals[1].recordOutputAndErrorCode("output\nstring two", 5);

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_task_vector:2\n"
                "my_task_vector_0:m_input_string:16:input string\none\n"
                "my_task_vector_0:m_output_string:17:output\nstring one\n"
                "my_task_vector_0:m_error_code:4\n"
                "my_task_vector_1:m_input_string:16:input string\ntwo\n"
                "my_task_vector_1:m_output_string:17:output\nstring two\n"
                "my_task_vector_1:m_error_code:5\n");
    }

    // Test serialising vector of pseudo random number generators
    {
        osstr.str("");

        std::string key("my_prng_vector");

        std::vector<std::mt19937_64> vals;

        vals.emplace_back(1);
        vals.emplace_back(2);

        std::ostringstream prng_sstr1;
        std::ostringstream prng_sstr2;
        prng_sstr1 << vals[0];
        prng_sstr2 << vals[1];

        serialise_vector(key, vals, osstr);

        assert(osstr.str() == "my_prng_vector:2\n"
                "my_prng_vector_0:" + prng_sstr1.str() + "\n"
                "my_prng_vector_1:" + prng_sstr2.str() + "\n"
                );
    }
}
