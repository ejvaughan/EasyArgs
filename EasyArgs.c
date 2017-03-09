#include "EasyArgs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "uthash.h"
#include <errno.h>

static void ParseOptionAndArgFromLine(char *line, ssize_t lineLength, char **outOption, char **outValue) {
	if (!line || !outOption || !outValue) {
		return;
	}

	char *optionStart = line;
	char *valueStart = NULL;
	char *valueEnd = NULL;
	
	// Skip over whitespace
	while (*optionStart && isspace(*optionStart)) {
		++optionStart;
	}

	if (*optionStart == '\0') {
		return;
	}

	char *optionEnd = optionStart;

	// Skip over nonwhitespace
	while (*optionEnd && !isspace(*optionEnd)) {
		++optionEnd;
	}

	if (*optionEnd) {
		if (optionEnd - line + 1 < lineLength) {
			valueStart = optionEnd + 1;
		}
		*optionEnd = '\0';
	}

	if (valueStart) {
		while (*valueStart && isspace(*valueStart)) {
			++valueStart;
		}

		valueEnd = valueStart;

		while (*valueEnd && !isspace(*valueEnd)) {
			++valueEnd;
		}

		if (valueEnd == valueStart) {
			valueStart = NULL;
		} else {
			*valueEnd = '\0';
		}
	}

	*outOption = optionStart;
	*outValue = valueStart;
}
	

static int ParseArgsFromConfigFile(CommandLineArgTemplate **templatesByName, CommandLineArgTemplate **templatesByLongName, const char *configFile, char **errorMessage) {
	if (!configFile) {
		return 0;
	}

	FILE *f = fopen(configFile, "r");
	if (!f) {
		if (errorMessage) {
			asprintf(errorMessage, "Unable to open configuration file %s", configFile);
		}
		return -1;
	}
	
	char *line = NULL;
	size_t n = 0;
	ssize_t lineLength;
	while ((lineLength = getline(&line, &n, f)) != -1) {
		char *option = NULL, *value = NULL;

		ParseOptionAndArgFromLine(line, lineLength, &option, &value);

		if (!option) {
			continue;
		}

		CommandLineArgTemplate *foundTemplate = NULL;
		HASH_FIND(nameHH, *templatesByName, option, strlen(option), foundTemplate);

		if (!foundTemplate) {
			HASH_FIND(longNameHH, *templatesByLongName, option, strlen(option), foundTemplate);
		}

		if (!foundTemplate) {
			// Skip over this option
			continue;
		}	

		foundTemplate->present = 1;

		if (foundTemplate->takesArg && !foundTemplate->value) {
			if (value) {
				foundTemplate->value = strdup(value);
			} else {
				if (errorMessage) {
					asprintf(errorMessage, "Error reading configuration file %s: option %s takes an argument\n", configFile, option);
				}
				free(line);
				return -1;
			}
		}
	}
	
	free(line);

	return 0;
}

void FreeCommandLineArgTemplateResources(CommandLineArgTemplate *templates[], int templatesCount)
{
	int i;
	for (i = 0; i < templatesCount; ++i) {
		if (templates[i]->value) {
			free(templates[i]->value);
		}
	}
}

int ParseCommandLineArgs(int argc, char *argv[], CommandLineArgTemplate *templates[], int templatesCount, CommandLineArgTemplate *configFileOptionTemplate, char *defaultConfigFile, char **outError)
{
	if (templatesCount <= 0) {
		return 0;
	}

	CommandLineArgTemplate *templatesByName = NULL;
	CommandLineArgTemplate *templatesByLongName = NULL;
	int i;
	int retVal = 0;

	// Build maps
	for (i = 0; i < templatesCount; ++i) {
		CommandLineArgTemplate *template = templates[i];
		if (template->name) {
			CommandLineArgTemplate *existing = NULL;
			HASH_FIND(nameHH, templatesByName, template->name, strlen(template->name), existing);
			if (existing) {
				// Failed precondition; templates' names should be unique
				goto exit;
			}
			HASH_ADD_KEYPTR(nameHH, templatesByName, template->name, strlen(template->name), template);
		}
		if (template->longName) {
			CommandLineArgTemplate *existing = NULL;
			HASH_FIND(longNameHH, templatesByLongName, template->longName, strlen(template->longName), existing);
			if (existing) {
				// Failed precondition; templates' long names should be unique
				goto exit;
			}
			HASH_ADD_KEYPTR(longNameHH, templatesByLongName, template->longName, strlen(template->longName), template);
		}
	}
	
	for (i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			// This arg is an option
			CommandLineArgTemplate *foundTemplate = NULL;

			char *optionName = "";
			if (argv[i][1] == '-') {
				optionName = argv[i] + 2;
			       	HASH_FIND(longNameHH, templatesByLongName, optionName, strlen(optionName), foundTemplate);
			} else {
				optionName = argv[i] + 1;
				HASH_FIND(nameHH, templatesByName, optionName, strlen(optionName), foundTemplate);
			}

			if (!foundTemplate) {
				// Unknown option
				if (outError) {
					asprintf(outError, "Error: unknown option: %s\n", optionName);
				}
				
				retVal = -1;
				goto exit;
			}

			foundTemplate->present = 1;

			// Parse the option's argument, if there is one
			if (foundTemplate->takesArg) {
				if (i < argc - 1 && argv[i + 1][0] != '-') {
					if (foundTemplate->value) {
						free(foundTemplate->value);
					}
					foundTemplate->value = strdup(argv[i + 1]);
					i += 1;
				} else {
					// Option should have an argument, but doesn't
					if (outError) {
						asprintf(outError, "Error: option %s takes an argument\n", optionName);
					}
					
					retVal = -1;
					goto exit;
				}
			}
		} else {
			// Stop parsing command line args
			retVal = i;
			break;
		}
	}

	// Read in arguments from config file, if necessary
	if (configFileOptionTemplate && configFileOptionTemplate->present) {
		if (ParseArgsFromConfigFile(&templatesByName, &templatesByLongName, configFileOptionTemplate->value, outError) < 0) {
			retVal = -1;
		}
	} else if (defaultConfigFile != NULL && access(defaultConfigFile, F_OK) == 0) {
		if (ParseArgsFromConfigFile(&templatesByName, &templatesByLongName, defaultConfigFile, outError) < 0) {
			retVal = -1;
		}
	}

	// Make sure that all the required options were supplied
	for (i = 0; i < templatesCount; ++i) {
		CommandLineArgTemplate *template = templates[i];
		if (template->required && !template->value) {
			if (outError) {
				char *optionName = (template->name) ? template->name : template->longName;
				asprintf(outError, "Option %s is required", optionName);
			}	
			retVal = -1;
			break;
		}
	}

	// Clean up
exit:
	HASH_CLEAR(nameHH, templatesByName);
	HASH_CLEAR(longNameHH, templatesByLongName);

	return retVal;
}

