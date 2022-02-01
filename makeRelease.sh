echo "old version=`git describe`"
fullversion=$1
version=${fullversion%%-*}
extra=-${fullversion##*-}
if [ $extra = "-$version" ]; then
    extra=""
fi
echo $version $extra
# strip any leading 0s from patch
branch=${version%.*}
major=${branch%.*}
minor=${branch#*.}
patch=${version##*.}
scidavis_version=$[$major*65536 + $minor*256 + $patch]
rm -f libscidavis/src/version.cpp
cat >libscidavis/src/version.cpp <<EOF
#include "globals.h"
const int SciDAVis::scidavis_versionNo = $scidavis_version;
const char* SciDAVis::scidavis_version = "$version";
const char* SciDAVis::extra_version = "$extra";
const char * SciDAVis::release_date = "`date +"%b %d, %Y"`";
EOF

# also check that all translation files have been included
for i in scidavis/translations/*.ts; do
    if git ls-files --error-unmatch >/dev/null; then
        true;
    else
        echo "translation $i not checked in"
        exit 1
    fi
done

# update Doxyversion
rm -f Doxyversion
echo "PROJECT_NUMBER=$version" >Doxyversion
echo "$version" >doc/version.tex

# update scidavis.appdata.xml
date=`date +%Y-%m-%d`
sed -i "s/<release version=.*$/<release version=\"$version\" date=\"$date\">/" scidavis/scidavis.appdata.xml

git commit -a -m "Release $fullversion"
git tag -a -m "" $fullversion
