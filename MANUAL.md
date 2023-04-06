# Return codes and errors
| Return Code   | Meaning               |
| ---           | ---                   |
| 0             | Correct execution     |
| 1             | Bad argument usage    |
| 2             | Bad `/etc/os-release` |
| -1            | Bad Environnment      |

Note: error 2 can be avoided by fixing the mentioned file or setting a value for `default_logo` in your configuration file. This error is linux-specific.
