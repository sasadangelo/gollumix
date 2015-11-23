# Gollumix 0.0.61

A simple Linux like kernel for educational purpose

## Why Gollumix?

Gollumix is a project I started just for fun when I was at the University and attended the Operating System course. Very quickly I realized that my teacher and text book explained a lot of useless theory on scheduling algorithms (usually 0.1% of an operating system) and very few information on device drivers (usually 80% of an operating system). Very soon, in the University library, I find out the Tanenbaum book "Operating Systems Design and Implementation" where the whole source code of an educational operating system was available: Minix. Immediately I started to study it. At that time Linux 2.0 was around for a while and I started to think to move to it.

Very soon I realized that the Linux 2.0 source code was not easy to understand and the documentation available was not enough. So I started to download the early versions of Linux (from 0.0.1 to 0.96a) and things became more clear. Very soon I find out that other people around the web started to build their own operating system for educational purpose. I started to think it could be a good idea to build my own educational operating system.

I decided to create a very easy operating system that was able to boot from floppy disk and show to the user a prompt where he could print whatever character he typed on the keyboard. I decided to have 4 local and 2 serial consoles and in order to manage them a minimal process management were necessary. Finally the kernel had to be able to read file from a floppy.

The result can be viewed downloading the source code below.

Create an operating system is not an easy task, it requires lot of effort to figure out how things work. For this reason I decided to build my operating system step by step and document each step in details. For more details see Documentation section below.

## Features

Check the feature list [here](https://github.com/sasadangelo/gollumix/wiki).

## Release and Download

The current pre-release is 0.0.61 and you can download it from [Github](https://github.com/sasadangelo/gollumix/releases).

## Documentation

Create an operating system is not an easy task, it requires lot of effort to figure out how things work. For this reason I decided to build my operating system step by step and document each step in details. For more details see Documentation [here](https://github.com/sasadangelo/gollumix/wiki).

## Contributing

Patches, forks, pull requests, suggestions or harsh flame is welcome! First of all read the [design document on the wiki](https://github.com/sasadangelo/gollumix/wiki/Design) to understand how the app works. I am preparing a TODO list and Contrubuting guidelines that will be available soon.

## License

This project is under GNU GPL v3. See LICENSE for more information.

----------------------------------------------------------------------------------------------
In this step we change the boot process (in particolar bootsect.S) in order
to be able to boot our kernel with a boot loader like LILO.
Basically we change the last byte of the boot sector according to the
LILO/GRUB BOOT protocol (see /usr/src/linux/Documentation/i386/boot.txt).
