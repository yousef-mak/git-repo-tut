name: C/C++ CI

on:
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Cache dependencies
      uses: actions/cache@v4
      id: cache-deps
      with:
        path: |
          /usr/lib/x86_64-linux-gnu/libgtest*
          /usr/include/gtest
          /usr/lib/x86_64-linux-gnu/libssl*
          /usr/lib/x86_64-linux-gnu/libcrypto*
          /usr/include/openssl
          ~/.cache/qt
        key: ${{ runner.os }}-deps-${{ hashFiles('**/CMakeLists.txt') }}
        restore-keys: |
          ${{ runner.os }}-deps-
    
    - name: Install Qt6
      uses: jurplel/install-qt-action@v3
      with:
        version: '6.5.0'
        cache: true
    
    - name: Install system dependencies
      if: steps.cache-deps.outputs.cache-hit != 'true'
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake build-essential
        sudo apt-get install -y libgtest-dev
        sudo apt-get install -y libssl-dev  # For password hashing
        
        # Build and install Google Test (only if not cached)
        cd /usr/src/gtest
        sudo cmake CMakeLists.txt
        sudo make
        sudo cp lib/*.a /usr/lib || sudo cp *.a /usr/lib
    
    - name: Cache CMake build
      uses: actions/cache@v4
      with:
        path: |
          build/
          !build/bin/
        key: ${{ runner.os }}-cmake-${{ hashFiles('**/CMakeLists.txt', 'src/**', 'include/**') }}
        restore-keys: |
          ${{ runner.os }}-cmake-
    
    - name: Configure CMake
      run: |
        cmake -B build -DCMAKE_BUILD_TYPE=Release
    
    - name: Build
      run: |
        cmake --build build --parallel $(nproc)
    
    - name: Run tests
      working-directory: build
      run: |
        ctest --output-on-failure
    
    - name: Upload build artifacts
      uses: actions/upload-artifact@v4
      with:
        name: build-artifacts
        path: |
          build/bin/
          build/*.exe
        retention-days: 7
