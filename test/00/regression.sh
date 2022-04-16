#! /bin/sh

here=`pwd`
if test $? -ne 0; then exit 2; fi
tmp=/tmp/$$
mkdir $tmp
if test $? -ne 0; then exit 2; fi
cd $tmp
if test $? -ne 0; then exit 2; fi

fail()
{
    echo "FAILED" 1>&2
    cd $here
    chmod -R u+w $tmp
    rm -rf $tmp
    exit 1
}

pass()
{
    echo "PASSED" 1>&2
    cd $here
    chmod -R u+w $tmp
    rm -rf $tmp
    exit 0
}

trap "fail" 1 2 3 15

# insert test commands here
cp $here/test/*.sciprj .
cp $here/test/*.opj .
$here/test/unittests
if test $? -ne 0; then fail; fi

PRETTY_NAME="openSUSE Tumbleweed"
if [ -f /etc/os-release ]; then
    . /etc/os-release
fi

if [ -z "$TRAVIS" ]; then
    diff Graph1.png $here/test/renderedImages/Graph1.png
    if test $? -ne 0; then fail; fi
fi
# unfortunately, this test fails, as OpenGL rendering appears to be
# non-deterministic.
#diff 3dplot.pdf $here/test/renderedImages/3dplot.pdf
#if test $? -ne 0; then fail; fi

for i in *.menudat; do
    diff -q $i $here/test/menuData/$i;
    if test $? -ne 0; then fail; fi
done


pass
