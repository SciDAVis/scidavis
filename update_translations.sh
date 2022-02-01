#!/bin/bash
# remove temporary directory
rm -rf scidavis/oldtranslations
# (re)create temporary directory
mkdir scidavis/oldtranslations
# copy old translation files
cp scidavis/translations/*.ts scidavis/oldtranslations/
# run lupdate to generate new files
lupdate -noobsolete scidavis.pro
# run python script to merge translated lines from old translations into new
python3 translation_updater.py scidavis/oldtranslations scidavis/translations
# remove temporary directory
rm -rf scidavis/oldtranslations