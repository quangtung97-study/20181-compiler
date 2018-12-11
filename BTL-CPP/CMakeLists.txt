cmake_minimum_required(VERSION 3.0)
project(compiler)

add_executable(main 
    main.cpp
    parser.cpp
    scope.cpp
    scanner.cpp
    reader.cpp
    assembly.cpp
    )

add_executable(main_scanner
    main_scanner.cpp
    scanner.cpp
    reader.cpp
    )

add_executable(main_reader
    main_reader.cpp
    reader.cpp
    )

add_executable(main_assembly
    main_assembly.cpp
    assembly.cpp
    )

add_executable(vm
    vm.cpp
    )
