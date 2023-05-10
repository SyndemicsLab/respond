#include "DataWriter.hpp"

namespace Data{

    /// @brief Default Constructor creating a completely empty DataWriter Object
    DataWriter::DataWriter() : DataWriter("", {}, {}, {}) {}

    /// @brief The main Constructor for DataWriter, it fills the object with the given parameters
    /// @param dirname A string containing a path to a directory to write output files
    /// @param interventions A list of Intervention State Names
    /// @param oudStates A list of OUD State Names
    /// @param demographics A List of Each Demographic List of Names
    /// @param history The history recorded from a Simulation Run
    DataWriter::DataWriter(std::string dirname,
        std::vector<std::string> interventions,
        std::vector<std::string> oudStates,
        std::vector<std::vector<std::string>> demographics) {
            this->dirname = dirname;
            this->interventions = interventions;
            this->oudStates = oudStates;
            this->demographics = demographics;
    }

    /// @brief Setter for Directory Path
    /// @param dirname String representing a path to a directory
    void DataWriter::addDirname(std::string dirname) {
        this->dirname = dirname;
    }

    /// @brief Getter for Directory Path
    /// @return String representation of Directory Path
    std::string DataWriter::getDirname() {
        return this->dirname;
    }

    /// @brief Setter for List of Intervention State Names
    /// @param interventions List of Intervention State Names
    void DataWriter::setInterventions(std::vector<std::string> interventions) {
        this->interventions = interventions;
    }

    /// @brief Setter for List of OUD State Names
    /// @param oudStates List of OUD State Names
    void DataWriter::setOUDStates(std::vector<std::string> oudStates) {
        this->oudStates = oudStates;
    }

    /// @brief Setter for List of Demographic Lists
    /// @param demographics List of List of Demographic Names
    void DataWriter::setDemographics(std::vector<std::vector<std::string>> demographics) {
        this->demographics = demographics;
    }

    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Data::FILE
    /// @return string containing the result if output enum is Data::STRING or description of status otherwise
    std::string DataWriter::writeHistory(OutputType outputType, History history) {
        if(history.stateHistory.getMatrices().empty() ||
            history.overdoseHistory.getMatrices().empty() ||
            history.fatalOverdoseHistory.getMatrices().empty() ||
            history.mortalityHistory.getMatrices().empty() ||
            this->dirname.empty()) {
            //log error
            std::ostringstream s;
            return s.str();
        }


        if (outputType == FILE) {
            if(!std::filesystem::exists(this->dirname)) {
                std::filesystem::create_directory(this->dirname);
            }

            std::filesystem::path dir(this->dirname);
            std::filesystem::path stateFile("stateHistory.csv");
            std::filesystem::path overdoseFile("overdoseHistory.csv");
            std::filesystem::path fatalOverdoseFile("fatalOverdoseHistory.csv");
            std::filesystem::path mortalityFile("mortalityHistory.csv");
            std::filesystem::path stateFullPath = dir/stateFile;
            std::filesystem::path overdoseFullPath = dir/overdoseFile;
            std::filesystem::path fatalOverdoseFullPath = dir/fatalOverdoseFile;
            std::filesystem::path mortalityFullPath = dir/mortalityFile;

            std::ofstream file;

            file.open(stateFullPath.string());
            this->writer(file, history.stateHistory);
            file.close();

            file.open(overdoseFullPath.string());
            this->writer(file, history.overdoseHistory);
            file.close();

            file.open(fatalOverdoseFullPath.string());
            this->writer(file, history.fatalOverdoseHistory);
            file.close();

            file.open(mortalityFullPath.string());
            this->writer(file, history.mortalityHistory);
            file.close();
            return "success";
        }
        std::ostringstream stringstream;
        this->writer(stringstream, history.stateHistory);
        this->writer(stringstream, history.overdoseHistory);
        this->writer(stringstream, history.fatalOverdoseHistory);
        this->writer(stringstream, history.mortalityHistory);
        return stringstream.str();
    }

    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Data::FILE
    /// @return string containing the result if output enum is Data::STRING or description of status otherwise
    std::string DataWriter::writeCost(OutputType outputType, Cost cost) {
        if(cost.healthcareCost.getMatrices().empty() ||
            cost.pharmaCost.getMatrices().empty() ||
            cost.fatalOverdoseCost.getMatrices().empty() ||
            cost.nonFatalOverdoseCost.getMatrices().empty() ||
            cost.treatmentCost.getMatrices().empty()) {
            //log error
            std::ostringstream s;
            return s.str();
        }


        if (outputType == FILE) {
            if(!std::filesystem::exists(this->dirname)) {
                std::filesystem::create_directory(this->dirname);
            }

            std::filesystem::path dir(this->dirname);
            std::filesystem::path healthUtilFile("healthcareCost.csv");
            std::filesystem::path pharmaFile("pharmaCost.csv");
            std::filesystem::path fatalOverdoseFile("fatalOverdoseCost.csv");
            std::filesystem::path nonFatalOverdoseFile("nonFatalOverdoseCost.csv");
            std::filesystem::path treatmentFile("treatmentCost.csv");
            std::filesystem::path healthUtilFullPath = dir/healthUtilFile;
            std::filesystem::path pharmaFullPath = dir/pharmaFile;
            std::filesystem::path fatalOverdoseFullPath = dir/fatalOverdoseFile;
            std::filesystem::path nonFatalOverdoseFullPath = dir/nonFatalOverdoseFile;
            std::filesystem::path treatmentFullPath = dir/treatmentFile;

            std::ofstream file;

            file.open(healthUtilFullPath.string());
            this->writer(file, cost.healthcareCost);
            file.close();

            file.open(pharmaFullPath.string());
            this->writer(file, cost.pharmaCost);
            file.close();

            file.open(fatalOverdoseFullPath.string());
            this->writer(file, cost.fatalOverdoseCost);
            file.close();

            file.open(nonFatalOverdoseFullPath.string());
            this->writer(file, cost.nonFatalOverdoseCost);
            file.close();

            file.open(treatmentFullPath.string());
            this->writer(file, cost.treatmentCost);
            file.close();
            return "success";
        }
        std::ostringstream stringstream;
        this->writer(stringstream, cost.healthcareCost);
        this->writer(stringstream, cost.pharmaCost);
        this->writer(stringstream, cost.fatalOverdoseCost);
        this->writer(stringstream, cost.nonFatalOverdoseCost);
        this->writer(stringstream, cost.treatmentCost);
        return stringstream.str();
    }

    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Data::FILE
    /// @return string containing the result if output enum is Data::STRING or description of status otherwise
    std::string DataWriter::writeUtility(OutputType outputType, Utility util) {
        if(util.backgroundUtility.getMatrices().empty() ||
            util.oudUtility.getMatrices().empty() ||
            util.settingUtility.getMatrices().empty()) {
            //log error
            std::ostringstream s;
            return s.str();
        }


        if (outputType == FILE) {
            if(!std::filesystem::exists(this->dirname)) {
                std::filesystem::create_directory(this->dirname);
            }

            std::filesystem::path dir(this->dirname);
            std::filesystem::path bgFile("backgroundUtility.csv");
            std::filesystem::path oudFile("oudUtility.csv");
            std::filesystem::path settingFile("settingUtility.csv"); 
            std::filesystem::path bgFullPath = dir/bgFile;
            std::filesystem::path oudFullPath = dir/oudFile;
            std::filesystem::path settingFullPath = dir/settingFile;

            std::ofstream file;

            file.open(bgFullPath.string());
            this->writer(file, util.backgroundUtility);
            file.close();

            file.open(oudFullPath.string());
            this->writer(file, util.oudUtility);
            file.close();

            file.open(settingFullPath.string());
            this->writer(file, util.settingUtility);
            file.close();

            return "success";
        }
        std::ostringstream stringstream;
        this->writer(stringstream, util.backgroundUtility);
        this->writer(stringstream, util.oudUtility);
        this->writer(stringstream, util.settingUtility);
        return stringstream.str();
    }
    

    /// @brief Helper function to write data to the specified stream
    /// @param stream Stream type to write data to
    /// @param historyToWrite Specific portion of history to save
    void DataWriter::writer(std::ostream &stream, Matrix3dOverTime historyToWrite) {
        std::vector<Matrix3d> Matrix3dVec = historyToWrite.getMatrices();
        stream << writeColumnHeaders(Matrix3dVec.size()) << std::endl;
        for(int i=0; i<this->interventions.size(); i++) {
            for(int j=0; j<this->oudStates.size(); j++) {
                for(int k=0; k<this->demographics.size(); k++) {
                    stream << this->interventions[i] << ",";
                    stream << this->oudStates[j] << ",";
                    for(int dem=0; dem<this->demographics[k].size(); dem++) {
                        stream << this->demographics[k][dem] << ",";
                    }
                    for(Matrix3d dm : Matrix3dVec) {
                        std::array<long int, 3> index = {0,0,0};
                        index[Data::INTERVENTION] = i;
                        index[Data::OUD] = j;
                        index[Data::DEMOGRAPHIC_COMBO] = k;
                        if(dm.NumDimensions != 3){
                            throw std::invalid_argument("Not 3 Dimensions in Matrix3d");
                        }
                        stream << std::to_string(dm(index[0], index[1], index[2])) << ",";
                    }
                    stream << std::endl;
                }
            }
        }
    }

    /// @brief Helper function to write Headers to CSVs
    /// @param timesteps Total duration incurred during the simulation
    /// @return String containing the CSV Column Headers
    std::string DataWriter::writeColumnHeaders(int timesteps) {
        std::string ret = "Interventions, OUD States,";
        for(int counter=0; counter<this->demographics[0].size(); counter++) {
            ret += fmt::format("Demographic {},", counter);
        }
        for(int timestep=0; timestep<timesteps; timestep++) {
            ret += fmt::format("t+{},", timestep);
        }
        return ret;
    }
}
