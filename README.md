#Atros

A Tiny Recreational Operating System  or  an attempt to create a small exokernel for the x86 Intel architecture in C.

## What is a kernel ? an exokernel ?

For an operating system, the kernel is the core of the system which manages the hardware (processor, memory, hard drive, graphic cards) and provides a secure and (most of the time) abstract way to use it to the higher level applications. A well-known example is the Linux kernel.

**Security** means that the kernel manages how ressources (such as memory) are accessed and shared among the applications.  
**Abstraction** means the kernel provides a way to access to these ressources without having to keep in consideration the true nature of the devices. A good example is the concept of *File System* which gives to the user the impression that no matter the device (HDD, SSD, USB key, ...) everything is files or folders.
  
An **Exokernel** tries to provide only security. This means applications have to deal with hardware management consideration. The main assumption of this design is that applications know better than the kernel how they will use the system resources and hence will benefit from the ability to manage these themselves.

More details about the concept of Exokernel are available at :

* [https://pdos.csail.mit.edu/exo.html](https://pdos.csail.mit.edu/exo.html)

and

* *Exokernel: An Operating System Architecture for Application-Level Resource Management*  
[https://pdos.csail.mit.edu/6.828/2008/readings/engler95exokernel.pdf](https://pdos.csail.mit.edu/6.828/2008/readings/engler95exokernel.pdf)


## How to compile and run

**Required software to compile the project:**  

 - binutils (ld)
 - gcc
 - make

The Makefile in the /src folder can be used to compile the whole project through the command :  
``` 
    make
```
 
**Required software to run the project**  

 - qemu
 - qemu-system-x86
 - xorriso
 - grub-pc-bin

The following command create an iso from the /iso folder and then start qemu with the resulting iso  file (atros.iso) as booting device.

```
    make qemu
```

If you wish to clean the project of the files resulting from the compilation use 
```
    make clean
```

## Project folders
