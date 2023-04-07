# Table of contents
1. [Configuration](#configuration)
    * [file location](#which-file-is-used)
    * [config syntax](#syntax)
2. [Command-line Arguments](#command-line-arguments)
3. [Return Codes](#return-codes-and-errors)
    * [0](#return-code-0)
    * [1](#error-1)
    * [-1](#error--1-255)



# Configuration

A complete configuration file with comments can be found as [`albafetch.conf`](albafetch.conf) in this repository.

## Which file is used?
Albafetch supports a series of configuration options that can be placed inside of a configuration file located in `$XDG_CONFIG_HOME/albafetch.conf` (`~/.config/albafetch.conf` is parsed if `XDG_CONFIG_HOME` is not set).

A custom file might be specified using the `--config` argument.

## Syntax
The config should contain `entry = "value",` pairs (using quotation marks is mandatory). The parser will look for the first string matching the entry (which you can find in [the default config](albafetch.conf)), then locate the next quotation mark, check whether there is another quotation mark following it and if so take the value between the two.

There are three different types of data that will be parsed:
* Strings: No more than N bytes between the quotation marks will be read, but every single one of those bytes will be used.
           These variables will be marked in the default config with a `; str [N]` following the option.
* Integers: An integer smaller or equal to N will be parsed using the C standard `atoi()` function (use `man 3 atoi` for specific information)
            These variables will be marked in the default config with a `; int [N]` following the option.
* Booleans: The program will recognize anything different from "false" as "true".
            These variables will be marked in the default config with a `; bool` following the option.

The parser will ignore anything that doesn't math what it is looking for, but it also allows explicit comments:
Anything between a `;` or a `#` and the end of the line will be completely ignored.

All of this means that `AB"CboldDEF"whatever lol"wo"w` will be parsed the exact same way as `bold = "true",` or `bold""`, but I would not reccommend this as it makes everything less readable.
I might also make the parser stricter in the future and configs written this way might stop working entirely.

# Command-line arguments
albafetch accepts a few command line arguments, which can be used to override certain values set in the config file.

A short explaination of what every argument does, including every accepted and default value, can be obtained by running `albafetch --help`

Here they are:
* `--help` (or `-h`): This option will print a small guide on the program usage and return 0.
* `--color` (or `-c`): This option, followed by the color you want to set, overrides the config `default_color` entry and sets a custom color for the whole output.
* `--bold` (or `-b`): This option, followed by a boolean "on" or "off", overrides the config `bold` antry and enables or disables the usage of bold in the output.
* `--logo` (or `-l`): This option, followed by the logo yo uwant to print, overrides the config `logo` entry and makes albafetch print a custom logo instead of the default one.
* `--align` (or `-a`): This option, followed by a boolean "on" or "off", overrides the config `align_infos` entry and enables of disables the alignment of the infos in the printed output.
* `--config`: This option, followed by a valid file path, changes the config file that will be parsed to look for a valid configuration.

# Return codes and errors
| Return Code   | Meaning               |
| ---           | ---                   |
| 0             | Correct execution     |
| 1             | Bad argument usage    |
| -1            | Bad Environnment      |

## Return code 0
As you probably already know, this is not an error, but it simply indicates that everything executed correctly. No need to worry.

## Error 1 
This is probably the result of a wrong usage of some command-line arguments. One or more error messages will be printed to stderr telling you exactly which argument(s) was used wrong.

## Error -1 (255)
This error should only pop up if the program was not able to get the `HOME` environnment variable, which should never happen. If you get this return code, you are probably experiencing worse than this dumb program not working.

---

### ©️ Aaron Blasko