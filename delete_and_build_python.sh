rm -rf bin/*
rm -rf build/*
rm -rf lib/*.a
rm -rf lib/*.so
cd scripts/
./debug-conanbuild.sh
cd ..