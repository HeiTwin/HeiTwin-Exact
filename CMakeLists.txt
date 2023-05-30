cmake_minimum_required(VERSION 3.10)

project(HeiTwin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(
  ./src
)


option(ARGTABLE3_ENABLE_TESTS "Enable unit tests" OFF)
option(ARGTABLE3_ENABLE_EXAMPLES "Enable examples" OFF)
add_subdirectory(./argtable argtable3)

# set(solvers greedy)
set(strategies greedy local_search)
set(data_structures graph)

add_library(utils src/utils.cc)
add_library(config src/config/config.cc)

foreach(file IN LISTS data_structures)
  add_library(${file} src/data_structures/${file}.cc)
endforeach()

# foreach(file IN LISTS solvers)
#   add_library(${file} src/solvers/${file}.cc)
# endforeach()

foreach(file IN LISTS strategies)
  add_library(strat_${file} src/strategies/${file}.cc)
endforeach()

foreach(file IN LISTS reductions)
  add_library(${file} src/reductions/${file}.cc)
endforeach()

add_executable(HeiTwin src/HeiTwin.cc)

target_link_libraries(HeiTwin PUBLIC config)
target_link_libraries(HeiTwin PUBLIC utils)
target_link_libraries(strat_greedy PUBLIC utils)

foreach(lib IN LISTS data_structures)
  target_link_libraries(HeiTwin PUBLIC ${lib})
endforeach()

# foreach(lib IN LISTS solvers)
#   target_link_libraries(HeiTwin PUBLIC ${lib})
# endforeach()

foreach(lib IN LISTS strategies)
  target_link_libraries(HeiTwin PUBLIC strat_${lib})
endforeach()

target_link_libraries(HeiTwin PUBLIC argtable3)
