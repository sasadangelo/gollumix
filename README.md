# Gollumix 0.0.61

A simple Linux like kernel for educational purpose

## Features

Check the feature list [here](https://github.com/sasadangelo/gollumix/wiki).

## Release

The current pre-release is 0.0.61 and you can download it from [Github](https://github.com/sasadangelo/gollumix/releases).

## Contributing

Patches, forks, pull requests, suggestions or harsh flame is welcome! First of all read the [design document on the wiki](https://github.com/sasadangelo/gollumix/wiki/Design) to understand how the app works. I am preparing a TODO list and Contrubuting guidelines that will be available soon.

## License

This project is under GNU GPL v3. See LICENSE for more information.

----------------------------------------------------------------------------------------------
In this step we change the boot process (in particolar bootsect.S) in order
to be able to boot our kernel with a boot loader like LILO.
Basically we change the last byte of the boot sector according to the
LILO/GRUB BOOT protocol (see /usr/src/linux/Documentation/i386/boot.txt).
