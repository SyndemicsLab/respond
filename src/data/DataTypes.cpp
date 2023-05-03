#include "DataTypes.hpp"

namespace Data {
    Matrix3dOverTime::Matrix3dOverTime(std::vector<Matrix3d> data){
        for(int i=0; i < data.size(); i++){
            this->data.insert({i, data[i]});
        }
    }

    Matrix3dOverTime::Matrix3dOverTime(std::vector<Matrix3d> data, std::vector<int> timestepChanges){
        if(data.size() != timestepChanges.size()){
            throw std::invalid_argument("Vector sizes are different");
        }

        for(int i=0; i < data.size(); i++){
            this->data.insert({timestepChanges[i], data[i]});
        }
    }

    Matrix3d Matrix3dOverTime::getMatrix3dAtTimestep(int timestep){
        int temp = this->data.begin()->first;
        for(auto itr=this->data.begin(); itr != this->data.end(); ++itr){
            int key = itr->first;
            if (key >= timestep){ return this->data[key]; }
        }
        return this->data.end()->second;
    }

    void Matrix3dOverTime::insert(Matrix3d datapoint, int timestep){
        this->data.insert({timestep, datapoint});
    }

    std::vector<Matrix3d> Matrix3dOverTime::getMatrices(){
        std::vector<Matrix3d> values;
        for(auto itr = this->data.begin(); itr != this->data.end(); itr++){
            Matrix3d mat = itr->second;
            values.push_back(mat);
        }
        return values;
    }
}
