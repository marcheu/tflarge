# TFlarge

##-Summary
TFlarge is a hack to allow using large tensorflow models on GPUs with limited VRAM.


## How to use
Go to the TFlarge directory, then:
    $ ./build
    $ export LD_PRELOAD=/path/to/tflarge.so
    [use tensor flow normally]


## Limitations
* TFlarge works by replacing allocations in VRAM with managed allocations.
  This means you are not limited by the amount of VRAM you have
* Because all allocations are managed, there is a performance hit, but it's
  still faster than running on the CPU. For example, it is faster to train
  large models on my GTX 1660 Super than on my 5950X CPU.

