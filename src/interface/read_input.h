#ifndef READ_INPUT_H
#define READ_INPUT_H

#include <string>
#include <vector>

void parse_csv_list(const std::string& csv_list, std::vector<std::string>& items);
void read_lines(std::istream& istrm, const int num_lines,
                std::vector<std::string>& lines);

#endif // READ_INPUT_H
