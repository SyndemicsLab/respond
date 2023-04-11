#include "DataWriter.hpp"


namespace Data{       

    DataWriter::DataWriter() : DataWriter("", {}, {}, {}, {}){}

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



    void DataWriter::AddHistory(History history){
        this->history = history;
    }


    void DataWriter::AddDirname(std::string dirname){
        this->dirname = dirname;
    }

    std::string DataWriter::getDirname(){
        return this->dirname;
    }

    void DataWriter::setInterventions(std::vector<std::string> interventions){
        this->interventions = interventions;
    }

    void DataWriter::setOUDStates(std::vector<std::string> oudStates){
        this->oudStates = oudStates;
    }

    void DataWriter::setDemographics(std::vector<std::vector<std::string>> demographics){
        this->demographics = demographics;
    }

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
            if(!boost::filesystem::exists(this->dirname)){
                boost::filesystem::create_directory(this->dirname);
            }

            boost::filesystem::path dir(this->dirname);
            boost::filesystem::path stateFile("stateHistory.csv");
            boost::filesystem::path overdoseFile("overdoseHistory.csv");
            boost::filesystem::path mortalityFile("mortalityHistory.csv");
            boost::filesystem::path stateFullPath = dir/stateFile;
            boost::filesystem::path overdoseFullPath = dir/overdoseFile;
            boost::filesystem::path mortalityFullPath = dir/mortalityFile;

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
                        index[Simulation::TREATMENT] = i;
                        index[Simulation::OUD] = j;
                        index[Simulation::DEMOGRAPHIC_COMBO] = k;
                        stream << dm(index[0], index[1], index[2]) << ",";
                    }
                    stream << std::endl;
                }
            }
        }
    }


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