/**
 * Functions for printing help info to the console.
 */

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
 * Prints information about how to format masks, invoked with "--help-mask"
 */
void print_help_mask();
