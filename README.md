# Overview

To parse command line args with EasyArgs, you create argument templates that specify which options are valid and if they take arguments. For example:

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

## Installation

TODO
