FLItest (flitest)
=================

A test program for [Finger Lakes Instrument](http://www.flicamera.com/) cameras and filterwheels.
Originally developed for the MicroLine 16803 camera and the CFW-4-5 filterwheel, but might be useful for
different cameras and filterwheels as well.

There are two executables, `flicamtest` and `fliflttest` to test the camera and the
filterwheel, respectively.

Requirements
------------

* libfli ([http://www.flicamera.com/software/index.html](http://www.flicamera.com/software/index.html))
* argtable2 ([http://argtable.sourceforge.net/](http://argtable.sourceforge.net/),
  `sudo apt-get install libargtable2-dev`)
* cfitsio ([http://heasarc.gsfc.nasa.gov/fitsio/](http://heasarc.gsfc.nasa.gov/fitsio/),
  `sudo apt-get install libcfitsio3-dev`)

Language
--------

C99.

Syntax
------
    flicamtest:
    flicamtest --info
    flicamtest --get-temperature
    flicamtest --set-temperature=N
    flicamtest --fan=(on|off)
    flicamtest --shutter=(open|close)
    flicamtest --acquire=<time> --shutter=(open|close) --output=xyz.fits [--bin=<bx,by>] [--offset=<x0,y0>] [--size=<sx,sy>] [--mode=(1mhz|8mhz)] [--verbose]
    flicamtest --help

    fliflttest:
    fliflttest --info
    fliflttest --set-filter=N
    fliflttest --home
    fliflttest --help

The explanation of the options can be acquired by `flicamtest --help` or `fliflttest --help`.

License
-------

WTFPL, see LICENSE.