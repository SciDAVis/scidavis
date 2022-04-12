fullversion=`git describe`
version=${fullversion%%-*}
extra=${fullversion##*-}
if [ $extra = $version ]; then
    name=scidavis-$version
else
    name=scidavis-$version~$extra
fi

git archive --format=tar --prefix=$name/ HEAD -o $name.tar

# add submodules
git submodule update --init --recursive
git submodule|cut -f3 -d' '|while read s; do
    pushd $s
    git archive --format=tar --prefix=$name/$s/ HEAD -o /tmp/$$.tar
    popd
    tar Af $name.tar /tmp/$$.tar
done
#if [ `ls scidavis/*.qm|wc -l` -gt 0 ]; then
#    tar --transform="s/^/$name\//" -rf $name.tar scidavis/*.qm
    gzip -f $name.tar
#else
#    echo "Please build scidavis to generate translations"
#fi
rm /tmp/$$.tar
