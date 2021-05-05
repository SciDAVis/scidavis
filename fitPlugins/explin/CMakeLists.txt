
add_library( explin SHARED "explin.c" )
target_link_libraries( explin GSL::gsl m)
