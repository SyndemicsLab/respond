#include "api.hpp"

int main(int argc, char **argv) {
    API::RESPONDAPI api;
    api.setupApp();
    api.runApp();
}