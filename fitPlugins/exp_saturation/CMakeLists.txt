
add_library( exp_saturation SHARED "exp_saturation.c" )
target_link_libraries( exp_saturation GSL::gsl m)
