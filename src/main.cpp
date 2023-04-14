#include "DataLoader.hpp"
#include "DataWriter.hpp"
#include "Simulation.hpp"

int main(int argc, char** argv) {
    using std::cout, std::cerr;
    if (argc != 3) {
        cerr << "Usage: " << argv[0] <<
             << "[INPUT FOLDER] [RUN NUMBER]\n\n"
             << "RESPOND, a compartmental simulation of healthcare in communities with high-risk opioid use"
    }
    Data::DataLoader inputs(argv[1]);
    Simulator::Sim sim(inputs);
    sim.Run();
    Data::DataWriter writer;
    writer.AddHistory(sim.History);
    writer.AddDirname("output");
    writer.Write(Data::FILE);
}