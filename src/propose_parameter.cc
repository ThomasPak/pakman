#include "types.h"
#include "system_call.h"
#include "propose_parameter.h"

void propose_parameter(const cmd_t& proposer,
                       const parameter_t& old_prmtr,
                       parameter_t& new_prmtr)
{

    using namespace std;

    system_call(proposer, old_prmtr, new_prmtr);
}
