name: C/C++ CI

on:
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

env:
  BUILD_TYPE: Release
  QT_VERSION: '6.5.0'

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v4
    
    # Cache Qt installation
    - name: Install Qt6
      uses: jurplel/install-qt-action@v3
      with:
        version: ${{ env.QT_VERSION }}
        cache: true
        cache-key-prefix: 'install-qt-action'
    
    # Cache system dependencies
    - name: Cache system dependencies
      uses: actions/cache@v4
      id: cache-system-deps
      with:
        path: |
          /var/cache/apt/archives
          /usr/lib/x86_64-linux-gnu/libgtest*
          /usr/include/gtest
          /usr/lib/x86_64-linux-gnu/libssl*
          /usr/include/openssl
        key: ${{ runner.os }}-system-deps-v2
        restore-keys: |
          ${{ runner.os }}-system-deps-
    
    # Only install if not cached
    - name: Install system dependencies
      if: steps.cache-system-deps.outputs.cache-hit != 'true'
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake build-essential libssl-dev
        
        # Install and build Google Test
        sudo apt-get install -y libgtest-dev
        cd /usr/src/gtest
        sudo cmake CMakeLists.txt
        sudo make
        sudo cp lib/*.a /usr/lib || sudo cp *.a /usr/lib
    
    # Cache CMake dependencies and build files
    - name: Cache CMake build
      uses: actions/cache@v4
      with:
        path: |
          build/
          !build/bin/
          !build/lib/
        key: ${{ runner.os }}-cmake-${{ hashFiles('**/CMakeLists.txt') }}-${{ hashFiles('src/**/*.cpp', 'src/**/*.h', 'include/**/*.h') }}
        restore-keys: |
          ${{ runner.os }}-cmake-${{ hashFiles('**/CMakeLists.txt') }}-
          ${{ runner.os }}-cmake-
    
    - name: Configure CMake
      run: |
        cmake -B build \
              -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }} \
              -DCMAKE_PREFIX_PATH=${{ env.Qt6_DIR }}
    
    - name: Build
      run: |
        cmake --build build --parallel $(nproc)
    
    - name: Run tests
      working-directory: build
      run: |
        ctest --output-on-failure --parallel $(nproc)
    
    - name: Upload build artifacts
      uses: actions/upload-artifact@v4
      with:
        name: build-artifacts
        path: |
          build/bin/
          build/*.exe
        retention-days: 7
        if-no-files-found: warn
