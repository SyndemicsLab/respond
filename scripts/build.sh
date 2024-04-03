module load python3/3.10.12
pip install conan
module load gcc/12.2.0

cd "$(git rev-parse --show-toplevel)"

$HOME/.local/bin/conan profile detect --force
$HOME/.local/bin/conan install . --build=missing --settings=build_type=Debug
cd build
source Debug/generators/conanbuild.sh
cmake .. -DCMAKE_TOOLCHAIN_FILE=Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build .
source Debug/generators/deactivate_conanbuild.sh