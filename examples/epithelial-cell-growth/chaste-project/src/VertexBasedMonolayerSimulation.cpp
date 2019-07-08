#include "SmartPointers.hpp"
#include "CellsGenerator.hpp"
#include "HoneycombVertexMeshGenerator.hpp"
#include "NagaiHondaForce.hpp"
#include "SimpleTargetAreaModifier.hpp"
#include "OffLatticeSimulation.hpp"
#include "UniformG1GenerationalCellCycleModel.hpp"
#include "VertexBasedCellPopulation.hpp"

// Generate cell cycle times exponentially
#include "FixedSequenceCellCycleModel.hpp"
#include "CellCycleTimesGenerator.hpp"

// System headers
#include <string>

// Own header
#include "VertexBasedMonolayerSimulation.hpp"

int VertexBasedMonolayerSimulation(
        const std::string& output_directory,
        double average_cell_cycle_time,
        double dt)
{

    /* The first thing we define is a 2D (specified by the <2,2>) mesh
     * which holds the spatial information of the simulation. To do this we
     * use one of a number of {{{MeshGenerators}}}.*/
    HoneycombVertexMeshGenerator generator(2, 2);
    MutableVertexMesh<2,2>* p_mesh = generator.GetMesh();

    /* We now generate a collection of cells. We do this by using a
     * {{{CellsGenerator}}} and we specify the proliferative behaviour of
     * the cell by choosing a {{{CellCycleModel}}}, here we choose a
     * {{{UniformG1GenerationalCellCycleModel}}} where each cell is given a
     * division time, drawn from a uniform distribution when it is created.
     * (Note that here we need to use a phase based cell cycle model so
     * that we can use the target area modifiers which are needed by the
     * vertex based simulations).  For a vertex simulation we need as may
     * cells as elements in the mesh.*/
    std::vector<CellPtr> cells;
    MAKE_PTR(TransitCellProliferativeType, p_transit_type);

    CellsGenerator<FixedSequenceCellCycleModel, 2> cells_generator;
    cells_generator.GenerateBasicRandom(cells, p_mesh->GetNumElements(), p_transit_type);

    /* We now create a {{{CellPopulation}}} object (passing in the mesh and
     * cells) to connect the mesh and the cells together.  Here that is a
     * {{{VertexBasedCellPopulation}}} and the dimension is <2>.*/
    VertexBasedCellPopulation<2> cell_population(*p_mesh, cells);

    // Set up cell cycle times generator
    CellCycleTimesGenerator::Instance()->SetRate(3.0 / (2.0*average_cell_cycle_time) );
    CellCycleTimesGenerator::Instance()->GenerateCellCycleTimeSequence();

    // adjust cell cycle times
    for (AbstractCellPopulation<2>::Iterator cell_iter =
            cell_population.Begin();
            cell_iter != cell_population.End();
            ++cell_iter)
    {
        FixedSequenceCellCycleModel* p_cell_cycle_model =
            static_cast<FixedSequenceCellCycleModel*>(cell_iter->GetCellCycleModel());
        p_cell_cycle_model->SetG2Duration(1.0/3.0*average_cell_cycle_time);

        // set M and S duration effectively to 0
        p_cell_cycle_model->SetMDuration(1e-12);
        p_cell_cycle_model->SetSDuration(1e-12);

        p_cell_cycle_model->Initialise();
    }

    /*
     * We now create an {{{OffLatticeSimulation}}} object and pass in the
     * {{{CellPopulation}}}. We also set some options on the simulation
     * like output directory, output multiple (so we don't visualize every
     * timestep), and end time.
     */
    OffLatticeSimulation<2> simulator(cell_population);
    simulator.SetOutputDirectory(output_directory);
    simulator.SetSamplingTimestepMultiple(200);
    simulator.SetEndTime(20.0);

    if (dt != DOUBLE_UNSET)
        simulator.SetDt(dt);

    /* To specify how cells move around, we create a "shared pointer" to a
     * {{{Force}}} object and pass it to the {{{OffLatticeSimulation}}}.
     * This is done using the MAKE_PTR macro as follows.
     */
    MAKE_PTR(NagaiHondaForce<2>, p_force);
    simulator.AddForce(p_force);

    /* A {{{NagaiHondaForce}}} has to be used together with a child class
     * of {{{AbstractTargetAreaModifier}}}.  This modifies the target area
     * of individual cells and thus alters the relative forces between
     * neighbouring cells.
     */
    MAKE_PTR(SimpleTargetAreaModifier<2>, p_growth_modifier);
    simulator.AddSimulationModifier(p_growth_modifier);

    /* Finally we call the {{{Solve}}} method on the simulation to run the
     * simulation.*/
    simulator.Solve();

    // Return number of cells
    return cell_population.GetNumRealCells();
}
