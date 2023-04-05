#ifndef SRC_DATALOADER_HPP_
#define SRC_DATALOADER_HPP_

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using InputTable = std::unordered_map<std::string, std::vector<std::string>>;

class DataLoader {
public:
    DataLoader() {};

    InputTable readCSV(const std::string&);
    void readInputDir(const std::string&);
};
#endif // SRC_DATALOADER_HPP_
