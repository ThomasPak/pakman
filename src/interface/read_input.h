#ifndef READ_INPUT_H
#define READ_INPUT_H

#include <string>
#include <vector>
#include "core/types.h"

void parse_csv_list(const std::string& csv_list, std::vector<std::string>& items);
void read_lines(std::istream& istrm, const int num_lines,
                std::vector<std::string>& lines);

/***** Rejection method *****/
namespace rejection
{

void read_input(std::istream& istrm, input_t& input_obj);

}

/***** MCMC method *****/
namespace mcmc
{

void read_input(std::istream& istrm, input_t& input_obj);

}

/***** SMC method *****/
namespace smc
{

void read_input(std::istream& istrm, input_t& input_obj);

}

/***** Parameter sweep *****/
namespace sweep
{

void read_input(std::istream& istrm, input_t& input_obj);

}

#endif // READ_INPUT_H
