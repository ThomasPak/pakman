#ifndef PARENTCOMMUNICATOR_HPP
#define PARENTCOMMUNICATOR_HPP

#include <string>
#include <stdexcept>
#include <mpi.h>

class ParentCommunicator {

    private:

        // Intercommunicator with parent
        MPI_Comm m_intercomm;

    public:

        // Create parent communicator
        ParentCommunicator();

        // Destoy parent communicator
        ~ParentCommunicator();

        // Receive message from parent
        std::string receiveMessage();

        // Send message to parent
        void sendMessage(const std::string& result);

        // Disconnect
        void disconnect();
};

ParentCommunicator::ParentCommunicator() {

    // Check if MPI has been initialized
    int flag;
    MPI_Initialized(&flag);

    if (!flag) {
        std::runtime_error e("MPI has to be initialized before creating ParentCommunicator");
        throw e;
    }

    // Get intercommunicator
    MPI_Comm_get_parent(&m_intercomm);

    if (m_intercomm == MPI_COMM_NULL) {
        std::runtime_error e("Not a valid parent communicator");
        throw e;
    }
}

ParentCommunicator::~ParentCommunicator() {}

std::string ParentCommunicator::receiveMessage() {

    // Source and tag are both zero
    const int source = 0, tag = 0;

    // Get length of message
    MPI_Status status; int count;
    MPI_Probe(source, tag, m_intercomm, &status);
    MPI_Get_count(&status, MPI_CHAR, &count);

    // Allocate buffer to store message
    char *buffer = new char[count];

    // Receive message
    MPI_Recv(buffer, count, MPI_CHAR, source, tag, m_intercomm, MPI_STATUS_IGNORE);

    // Create string and delete buffer
    std::string message(buffer);
    delete[] buffer;

    return message;
}

void ParentCommunicator::sendMessage(const std::string& message) {

    // Destination and tag are both zero
    const int dest = 0, tag = 0;

    // Send message
    MPI_Send(message.c_str(), message.size() + 1, MPI_CHAR, dest, tag, m_intercomm);
}

void ParentCommunicator::disconnect() {
    MPI_Comm_disconnect(&m_intercomm);
}

#endif // PARENTCOMMUNICATOR_HPP
