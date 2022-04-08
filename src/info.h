#ifndef INFO_H
#define INFO_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void separator();   // prints a separator

void title();       // prints a title in the format user@hostname

void hostname();    // getting the computer hostname

void user();        // get the currently logged user

void uptime();      // prints the uptime

void os();          // prints the os name + arch

void kernel();      // prints the kernel version

void desktop();     // prints the current desktop environment

void shell();       // prints the user default shell

void term();        // prints the current terminal

void packages();    // prints the number of installed packages

void host();        // prints the current host machine

void cpu();         // prints the current CPU

void gpu();         // prints the current GPU

void memory();      // prints the used memory in the format used MiB / total MiB (XX%)

void public_ip();   // get the public IP adress

void local_ip();    // get the local IP adress - WORK IN PROGRESS

#endif