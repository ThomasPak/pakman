#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
    // Process arguments
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] <<
            " M Nx Ny\n"
            "\n"
            "Solve heat equation on the unit square for a single unit of time.\n"
            "The initial temperature profile is set to 1 over the whole domain.\n"
            "\n"
            "Space is discretised using the finite difference method.\n"
            "Time is discretised using the forward Euler method.\n"
            "\n"
            "The domain is discretised by a (Nx + 1)-by-(Ny + 1) grid.\n"
            "Time is discretised into M timesteps.\n";

        return 2;
    }

    int M = std::stoi(argv[1]);
    int Nx = std::stoi(argv[2]);
    int Ny = std::stoi(argv[3]);

    std::cerr << "M: " << M << std::endl;
    std::cerr << "Nx: " << Nx << std::endl;
    std::cerr << "Ny: " << Ny << std::endl;

    // Define constants
    double dt = 1.0 / M;
    double dx = 1.0 / Nx;
    double dy = 1.0 / Ny;
    double mux = dt / (dx * dx);
    double muy = dt / (dy * dy);
    double nu = 1.0 - 2 * mux - 2 * muy;

    std::cerr << "dt: " << dt << std::endl;
    std::cerr << "dx: " << dx << std::endl;
    std::cerr << "dy: " << dy << std::endl;

    std::cerr << "mux: " << mux << std::endl;
    std::cerr << "muy: " << muy << std::endl;
    std::cerr << "nu: " << nu << std::endl;

    // Check stability criteria
    std::cerr << "dt: " << dt << std::endl;
    std::cerr << "0.125 * (dx * dx + dy * dy): "
        << 0.125 * (dx * dx + dy * dy) << std::endl;
    if (dt >= 0.125 * (dx * dx + dy * dy))
    {
        std::cerr << "Warning: dt is too small, the method may be unstable.\n";
        std::cerr << "Choose a larger M to avoid this warning.\n";
    }

    // Allocate vectors to store solution in
    std::vector<double> v1((Nx + 1) * (Ny + 1), 0.0);
    std::vector<double> v2((Nx + 1) * (Ny + 1), 0.0);

    // Initial temperature profile is constant with value 1
    // Iterate in j (y-axis)
    for (int j = 1; j <= Ny - 1; j++)
    {
        // Iterate in i (x-axis)
        for (int i = 1; i <= Nx - 1; i++)
        {
            int idx = i + j * (Nx + 1);
            v1[idx] = 1.0;
        }
    }

    // Explicit Euler method time stepping
    for (int m = 1; m <= M; m++)
    {
        // Iterate in j (y-axis)
        for (int j = 1; j <= Ny - 1; j++)
        {
            // Iterate in i (x-axis)
            for (int i = 1; i <= Nx - 1; i++)
            {
                int idx = i + j * (Nx + 1);
                v2[idx] =
                    nu * v1[idx] +
                    mux * (v1[idx - 1] + v1[idx + 1]) +
                    muy * (v1[idx - (Nx + 1)] + v1[idx + (Nx + 1)]);
            }
        }

        // Swap vectors so new solution is now in v1
        std::swap(v1, v2);
    }

    // Print out solution
    // Iterate in j (y-axis)
    for (int j = 0; j <= Ny; j++)
    {
        // Iterate in i (x-axis)
        for (int i = 0; i <= Nx; i++)
        {
            int idx = i + j * (Nx + 1);
            std::cout << v1[idx] << " ";
        }

        std::cout << std::endl;
    }
}
