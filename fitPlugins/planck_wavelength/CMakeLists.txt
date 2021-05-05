
add_library( planck_wavelength SHARED "planck_wavelength.c" )
target_link_libraries( planck_wavelength GSL::gsl m)
