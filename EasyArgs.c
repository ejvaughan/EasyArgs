#include "EasyArgs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uthash.h"
#include <errno.h>

static void ParseArgsFromConfigFile(CommandLineArgTemplate **templatesByName, CommandLineArgTemplate **templatesByLongName, const char *configFile) {
	if (!configFile) {
		return;
	}

	FILE *f = fopen(configFile, "r");
	if (!f) {
		return;
	}
	
	char *line = NULL;
	size_t length = 0;
	while (getline(&line, &length, f) != -1) {
		char *option = NULL, *arg = NULL;

		int numScanned;
		if ((numScanned = sscanf(line, "%ms %ms", &option, &arg)) >= 1) {
			CommandLineArgTemplate *foundTemplate = NULL;
			HASH_FIND(nameHH, *templatesByName, option, strlen(option), foundTemplate);

			if (!foundTemplate) {
			       	HASH_FIND(longNameHH, *templatesByLongName, option, strlen(option), foundTemplate);
			}

			if (!foundTemplate) {
				// Skip over this option
				free(option);
				free(arg);
				continue;
			}	

			foundTemplate->present = 1;

			if (foundTemplate->takesArg && !foundTemplate->value) {
				if (numScanned == 2) {
					foundTemplate->value = arg;
					arg = NULL;
				} else {
					printf("Error reading configuration file %s: option %s takes an argument\n", configFile, option);
					exit(EXIT_FAILURE);
				}
			}
		}

		free(option);
		free(arg);
	}
	
	free(line);
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

int ParseCommandLineArgs(int argc, char *argv[], CommandLineArgTemplate *templates[], int templatesCount, CommandLineArgTemplate *configFileOptionTemplate, char *defaultConfigFile)
{
	CommandLineArgTemplate *templatesByName = NULL;
	CommandLineArgTemplate *templatesByLongName = NULL;
	int i;

	// Build maps
	for (i = 0; i < templatesCount; ++i) {
		CommandLineArgTemplate *template = templates[i];
		HASH_ADD_KEYPTR(nameHH, templatesByName, template->name, strlen(template->name), template);
		HASH_ADD_KEYPTR(longNameHH, templatesByLongName, template->longName, strlen(template->longName), template);
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
				printf("Error: unknown option: %s\n", optionName);
				exit(EXIT_FAILURE);
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
					printf("Error: option %s takes an argument\n", optionName);
					exit(EXIT_FAILURE);
				}
			}
		} else {
			// Stop parsing command line args
			break;
		}
	}

	// Read in arguments from config file, if necessary
	if (configFileOptionTemplate && configFileOptionTemplate->present) {
		ParseArgsFromConfigFile(&templatesByName, &templatesByLongName, configFileOptionTemplate->value);
	} else if (defaultConfigFile != NULL) {
		ParseArgsFromConfigFile(&templatesByName, &templatesByLongName, defaultConfigFile);
	}

	// Clean up
	HASH_CLEAR(nameHH, templatesByName);
	HASH_CLEAR(longNameHH, templatesByLongName);

	return i;
}

