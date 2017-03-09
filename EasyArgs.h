#ifndef __EASY_ARGS_H
#define __EASY_ARGS_H
#include "uthash.h"

typedef struct CommandLineArgTemplate {
	char *description;
	char *name;
	char *longName;
	int required;
	int takesArg;
	int present; // Indicates whether the option was supplied. Set by `ParseCommandLineArgs`.
	char *value; // Filled in by `ParseCommandLineArgs` with the option's value.
	UT_hash_handle nameHH; 		// Private
	UT_hash_handle longNameHH; 	// Private
} CommandLineArgTemplate;

/**
 * Creates a template for a command line argument.
 *
 * A template must have either a name or a long name, or both.
 *
 * @param name The name of the option. An option with name `f` is supplied on the command line with a single dash, `-f`. May be NULL.
 * @param longName The "long name" of the option. An option with long name `f` is supplied on the command line with two dashes, e.g. `--f`. May be NULL.
 * @param required Whether this option must be supplied in order to successfully parse the args. If a required option is missing, `ParseCommandLineArgs` returns a negative value indiciating an error.
 * @param takesArg Whether or not this option takes an argument. An option's argument is supplied immediately after the option, e.g. `-f <arg>`.
 * @param description A human-readable description of this option. May be NULL.
 */
#define CreateArgTemplate(name, longName, required, takesArg, description) \
{ description, name, longName, required, takesArg, 0, NULL }

/**
 * Parses command line arguments according to the provided templates.
 *
 * The following scenarios define an unsuccessful parse:
 *  - A required option is missing
 *  - An option is missing its argument
 *  - An unknown option is present (only checked for arguments in `argv`; see below)
 *  
 * If parsing succeeds, each template's `present` and `value` members will be set accordingly. For example, if `foo` is a template passed to the API, `foo.present` will be set if the option is supplied. This is useful to test for the presence of options that are not required. In addition, for options that take an argument, the argument's value is accessibile via `foo.value`.
 *
 * In addition to parsing arguments provided via the command line, this API has the ability to read the arguments from a file. The file can be either a file in the current directory, or one that the user specifies on the command line via a special option. In either case, the format of the file must be as follows:
 *
 *     option1 value1
 *     option2
 *     option3 value3
 *     ...
 *
 * where option1 and option3 take an argument, and option2 does not. The options are supplied without a preceding `-` or `--`.
 *
 * If an option that takes an argument is supplied both on the command line and in a file, then the value supplied on the command line will take precedence.
 *
 * The ability to supply options via a file in addition to the command line provides automatic support for using a "configuration" file to control the program's behavior. This has the added benefit that the program's options are uniform across the configuration file and the command line. It's possible for the configuration file to supply only a portion of the options, with the others being supplied on the command line. Alternatively, the options present in the configuration file may serve as sensible defaults for the program, which can be overridden if necessary on the command line.
 *
 * Important: Make sure to call `FreeCommandLineArgTemplateResources` after you are done with the argument templates. Failure to call this function may result in a memory leak.
 *
 * @param argc The number of command line arguments in `arg`.
 * @param argv The command line arguments to be parsed.
 * @param templates An array of argument template references that will guide the parsing process.
 * @param templatesCount The number of templates supplied in the `templates` parameter.
 * @param configFileOptionTemplate Optional. If supplied, it is a pointer to one of the argument templates in the `templates` array. This option must take an argument, and its value will be interpreted as the path to a file from which command line arguments should be read in.
 * @param defaultConfigFile The name of a file in the current directory from which command line arguments will be read.
 * @param outError Upon an error, *outError will be set to an error message that should be displayed to the user. Important: This string must be released using free().
 * @return Upon success, returns the index of the command line argument at which parsing stopped. Upon error, returns a negative value and sets *outError to an error message which should be displayed to the user. Important: *outError must be released via a call to free() after you are done with it
 */
int ParseCommandLineArgs(int argc, char *argv[], CommandLineArgTemplate *templates[], int templatesCount, CommandLineArgTemplate *configFileOptionTemplate, char *defaultConfigFile, char **outError);

/**
 * Frees any resources allocated to the argument templates by `ParseCommandLineArgs`.
 *
 * @param templates The array of templates that was passed to `ParseCommandLineArgs`.
 * @param templatesCount The number of templates in the templates array.
 */
void FreeCommandLineArgTemplateResources(CommandLineArgTemplate *templates[], int templatesCount);

#endif /* EasyArgs.h */
