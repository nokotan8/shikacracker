#include <string>

/**
 * Prints general help, invoked with "-h/--help"
 * arg0: command that ran this program, e.g. (./)shikacracker
 */
void print_help_general(char *arg0);

/**
 * Prints a list of available hash modes, invoked with "--help-hash"
 */
void print_help_hash();

/**
 * Prints a list of available attack modes, invoked with "--help-mode"
 */
void print_help_mode();

/**
 * Remove leading and trailing whitespace from a string.
 * Taken from https://stackoverflow.com/a/1798170
 */
std::string trim_wspace(const std::string &str,
                        const std::string &whitespace = " \t");
