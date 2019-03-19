#ifndef SWEEPCONTROLLER_H
#define SWEEPCONTROLLER_H

#include <vector>
#include <string>

#include "types.h"
#include "LongOptions.h"
#include "Arguments.h"
#include "Parameter.h"

#include "AbstractController.h"

class SweepController : public AbstractController
{
    public:

        // Constructor
        SweepController(const sweep::input_t &input_obj);

        // Default destructor
        virtual ~SweepController() override = default;

        // Iterate function
        virtual void iterate() override;

        // Simulator getter
        virtual cmd_t getSimulator() const override;

        // Static help function
        static std::string help();

        // Static addLongOptions function
        static void addLongOptions(LongOptions& lopts);

        // Static function to make from positional arguments
        static SweepController* makeController(const Arguments& args);

    private:

        /**** Member variables ****/
        // Parameter names
        std::vector<std::string> m_parameter_names;

        // Parameter list
        std::vector<Parameter> m_prmtr_list;

        // Counter for number of finished parameters
        int m_num_finished = 0;

        // At first iteration, SweepController will push all parameters to the
        // pending queue of the Master.  This flag tells iterate() that it is
        // in the first iteration.
        bool m_first_iteration = true;

        // Simulator command
        cmd_t m_simulator;
};

#endif // SWEEPCONTROLLER_H
