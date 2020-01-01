
### References

* https://grpc.io

## How To

### Build

Simply run with the CMake. Its version must be **3.14 or later**.

```cmake
cmake_minimum_required(VERSION 3.14)
```

#### VcPkg

The project requires [VcPkg](https://github.com/microsoft/vcpkg). Expects 2019.08 release or later

```console
$ vcpkg install --triplet x64-windows grpc catch2 ms-gsl
```

After the installation, run CMake with the `CMAKE_TOOLCHAIN_FILE`.

```console
$ cd /code/build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE="/vcpkg/scripts/buildsystem/vcpkg.cmake"
$ cmake --build . --config debug
```

### Code generation for gRPC 

There are several **CMake targets** to support code generations. Each can be used like the following

```console
$ cd /code/build
$ cmake --build . --target generate_grpc_files_cpp
```

Current codegen target list:
* `codegen_cpp`
* `codegen_python`
* `codegen_nodejs`
* `codegen_csharp`
* `codegen_go`

Especially, Go requires additional setup to acquire code generator.

```sh
export GOPATH=$(pwd)
go get github.com/golang/protobuf/protoc-gen-go
```

Or, for PowerShell, 

```ps1
$env:GOPATH=Get-Location
go get github.com/golang/protobuf/protoc-gen-go
```

By doing this, 'bin/' folder will contain 'protoc-gen-go' executable.

### Test

CTest is enabled

> TBA

