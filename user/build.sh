mkdir -p build/android && cd build/android
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_PLATFORM=android-33 \
    -DANDROID_ABI=arm64-v8a ../..
cmake --build .