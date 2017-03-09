# Overview

To parse command line args with EasyArgs, you create argument templates that specify which options are valid and if they take arguments. Then, you simply pass them to `ParseCommandLineArgs`. For example:

    // Defines a required option -f / --foo that takes an argument
    CommandLineArgTemplate optionFoo = CreateArgTemplate("f", "foo", 1, 1, NULL);
    
    CommandLineArgTemplate *templates = { &optionFoo };
    int numTemplates = sizeof(templates)/sizeof(templates[0]);

    char *errorMessage = NULL;
    if (ParseCommandLineArgs(argc, argv, templates, numTemplates, NULL, NULL, &errorMessage) < 0) {
        printf("Error: %s\n", errorMessage);
        
        free(errorMessage);
        exit(EXIT_FAILURE);
    }

    // Access the option's argument
    printf("%s\n", optionFoo.value);

An argument template is created using the `CreateArgTemplate` macro:
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

The following scenarios define an unsuccessful parse:

* A required option is missing
* An option is missing its argument
* An unknown option is present (only checked for arguments in `argv`; see below)
   
If parsing succeeds, each template's `present` and `value` members will be set accordingly. For example, if `foo` is a template passed to the API, `foo.present` will be set if the option is supplied. This is useful to test for the presence of options that are not required. In addition, for options that take an argument, the argument's value is accessibile via `foo.value`.

In addition to parsing arguments provided via the command line, EasyArgs has the ability to read the arguments from a file. The file can be either a file in the current directory, or one that the user specifies on the command line via a special option. In either case, the format of the file must be as follows:
     option1 value1
     option2
     option3 value3
     ...

where option1 and option3 take an argument, and option2 does not. The options are supplied without a preceding `-` or `--`.

If an option that takes an argument is supplied both on the command line and in a file, then the value supplied on the command line will take precedence.

The ability to supply options via a file in addition to the command line provides automatic support for using a "configuration" file to control the program's behavior. This has the added benefit that the program's options are uniform across the configuration file and the command line. It's possible for the configuration file to supply only a portion of the options, with the others being supplied on the command line. Alternatively, the options present in the configuration file may serve as sensible defaults for the program, which can be overridden if necessary on the command line.

## Installation

TODO
