#include "DataWriter.hpp"


namespace Data{       

    /// @brief Default Constructor creating a completely empty DataWriter Object
    DataWriter::DataWriter() : DataWriter("", {}, {}, {}, {}){}

    /// @brief The main Constructor for DataWriter, it fills the object with the given parameters
    /// @param dirname A string containing a path to a directory to write output files
    /// @param interventions A list of Intervention State Names
    /// @param oudStates A list of OUD State Names
    /// @param demographics A List of Each Demographic List of Names
    /// @param history The history recorded from a Simulation Run
    DataWriter::DataWriter(std::string dirname, 
        std::vector<std::string> interventions, 
        std::vector<std::string> oudStates, 
        std::vector<std::vector<std::string>> demographics,
        History history){
            this->dirname = dirname;
            this->interventions = interventions;
            this->oudStates = oudStates;
            this->demographics = demographics;
            this->AddHistory(history);
    }


    /// @brief Setter for History from a Simulation
    /// @param history History to write to a CSV
    void DataWriter::AddHistory(History history){
        this->history = history;
    }

    /// @brief Setter for Directory Path
    /// @param dirname String representing a path to a directory
    void DataWriter::AddDirname(std::string dirname){
        this->dirname = dirname;
    }

    /// @brief Getter for Directory Path
    /// @return String representation of Directory Path
    std::string DataWriter::getDirname(){
        return this->dirname;
    }

    /// @brief Setter for List of Intervention State Names
    /// @param interventions List of Intervention State Names
    void DataWriter::setInterventions(std::vector<std::string> interventions){
        this->interventions = interventions;
    }

    /// @brief Setter for List of OUD State Names
    /// @param oudStates List of OUD State Names
    void DataWriter::setOUDStates(std::vector<std::string> oudStates){
        this->oudStates = oudStates;
    }

    /// @brief Setter for List of Demographic Lists
    /// @param demographics List of List of Demographic Names
    void DataWriter::setDemographics(std::vector<std::vector<std::string>> demographics){
        this->demographics = demographics;
    }

    /// @brief Main Operation of Class, Write data to output
    /// @param outputType Output Enum, generally Data::FILE
    /// @return string containing the result if output enum is Data::STRING or description of status otherwise
    std::string DataWriter::Write(OutputType outputType){
        if(this->history.stateHistory.empty() ||
            this->history.overdoseHistory.empty() ||
            this->history.mortalityHistory.empty() ||
            this->dirname.empty()){
            //log error
            std::ostringstream s;
            return s.str();
        }
        

        if (outputType == FILE){
            if(!std::filesystem::exists(this->dirname)){
                std::filesystem::create_directory(this->dirname);
            }

            std::filesystem::path dir(this->dirname);
            std::filesystem::path stateFile("stateHistory.csv");
            std::filesystem::path overdoseFile("overdoseHistory.csv");
            std::filesystem::path mortalityFile("mortalityHistory.csv");
            std::filesystem::path stateFullPath = dir/stateFile;
            std::filesystem::path overdoseFullPath = dir/overdoseFile;
            std::filesystem::path mortalityFullPath = dir/mortalityFile;

            std::ofstream file;
            
            file.open(stateFullPath.string());
            this->Writer(file, this->history.stateHistory);
            file.close();

            file.open(overdoseFullPath.string());
            this->Writer(file, this->history.overdoseHistory);
            file.close();

            file.open(mortalityFullPath.string());
            this->Writer(file, this->history.mortalityHistory);
            file.close();
            return "success";
        }
        std::ostringstream stringstream;
        this->Writer(stringstream, this->history.stateHistory);
        this->Writer(stringstream, this->history.overdoseHistory);
        this->Writer(stringstream, this->history.mortalityHistory);
        return stringstream.str();
    }

    /// @brief Helper function to write data to the specified stream
    /// @param stream Stream type to write data to
    /// @param historyToWrite Specific portion of history to save
    void DataWriter::Writer(std::ostream &stream, Matrix3dOverTime historyToWrite){
        stream << WriteColumnHeaders(historyToWrite.size()) << std::endl;
        for(int i=0; i<this->interventions.size(); i++){
            for(int j=0; j<this->oudStates.size(); j++){
                for(int k=0; k<this->demographics.size(); k++){
                    stream << this->interventions[i] << ",";
                    stream << this->oudStates[j] << ",";
                    for(int dem=0; dem<this->demographics[k].size(); dem++){
                        stream << this->demographics[k][dem] << ",";
                    }
                    for(Matrix3d dm : historyToWrite){
                        std::array<long, 3> index = {0,0,0};
                        index[Simulation::INTERVENTION] = i;
                        index[Simulation::OUD] = j;
                        index[Simulation::DEMOGRAPHIC_COMBO] = k;
                        stream << dm(index[0], index[1], index[2]) << ",";
                    }
                    stream << std::endl;
                }
            }
        }
    }

    /// @brief Helper function to write Headers to CSVs
    /// @param timesteps Total duration incurred during the simulation
    /// @return String containing the CSV Column Headers
    std::string DataWriter::WriteColumnHeaders(int timesteps){
        std::string ret = "Interventions, OUD States,";
        for(int counter=0; counter<this->demographics[0].size(); counter++){
            ret += fmt::format("Demographic {},", counter);
        }
        for(int timestep=0; timestep<timesteps; timestep++){
            ret += fmt::format("t+{},", timestep);
        }
        return ret;
    }


}