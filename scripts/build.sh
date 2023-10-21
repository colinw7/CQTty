#!/bin/csh -f

# Instructions
#  . make empty build directory
#  . cp build.sh to empty build directory
#  . Run ./build.sh
#
# Extra (system) dependencies
#  jpeg, png, tre, curses

set cdir = `pwd`

if (! -e CQTty) then
  git clone https://github.com/colinw7/CQTty.git CQTty
endif

set inc = `cat CQTty/src/CQTty.pro | sed ':x; /\\$/ { N; s/\\\n//; tx }' | grep unix:LIBS | sed 's/unix:LIBS += //'`

set dirs = ()

foreach v ($inc)
  set n = `echo $v | grep '..\/..\/' | wc -l`

  if ($n != 1) then
    continue
  endif

  set v1 = `echo $v | sed 's@-L../../\(.*\)/lib@\1@'`

  set dirs = ($dirs $v1)

  if (! -e $v1) then
    echo "--- Clone $v1 ---"

    git clone https://github.com/colinw7/${v1}.git $v1
  endif
end

set edirs = (CImageLib CMath CGlob)

foreach dir ($edirs)
  if (! -e $dir) then
    echo "--- Clone $v1 ---"

    git clone https://github.com/colinw7/${dir}.git $dir
  endif
end

foreach dir ($dirs)
  echo "--- Build $dir ---"

  if (-e $cdir/$dir/src) then
    cd $cdir/$dir/src

    if (! -e ../obj) then
      mkdir ../obj
    endif

    if (! -e ../lib) then
      mkdir ../lib
    endif

    set n = `find . -maxdepth 1 -name "*.pro" | wc -l`

    if ($n == 1) then
      qmake
    endif

    make
  endif
end

# extra libs
set edirs = ()

foreach edir ($edirs)
  echo "--- Build $edir ---"

  if (-e $cdir/$edir) then
    cd $cdir/$edir

    if (! -e ../obj) then
      mkdir ../obj
    endif

    if (! -e ../lib) then
      mkdir ../lib
    endif

    set n = `find . -maxdepth 1 -name "*.pro" | wc -l`

    if ($n == 1) then
      qmake
    endif

    make
  endif
end

echo "--- Build CQTty (src) ---"

cd $cdir/CQTty/src

qmake

make
