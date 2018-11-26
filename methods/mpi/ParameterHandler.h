#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

// TODO: Implement Parameter class that is constructed from
// parameter_t and combines the functionality of parameter_t,
// status_t, holds a buffer for MPI_Isend, automatically
// deallocates said buffer.

#include "common.h"
#include "../types.h"

class ParameterHandler {

    public:

        // Construct from parameter
        ParameterHandler(parameter_t prmtr) : m_prmtr(prmtr), m_status(busy) {}

        // Get parameter
        parameter_t getParameter() const { return m_prmtr; }

        // Get c string
        const char* getCString() const { return m_prmtr.c_str(); }

        // Get parameter size
        size_t getParameterSize() const { return m_prmtr.size(); }

        // Get status
        status_t getStatus() const { return m_status; }

        // Set status
        void setStatus(status_t new_status) { m_status = new_status; }

    private:

        const parameter_t m_prmtr;
        status_t m_status;
};

#endif // PARAMETERHANDLER_H
