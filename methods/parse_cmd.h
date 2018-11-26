#ifndef PARSE_CMD_H
#define PARSE_CMD_H

#include <vector>
#include <string>
#include "types.h"

void parse_cmd(const cmd_t& cmd, std::vector<std::string>& cmd_tokens);

#endif // PARSE_CMD_H
