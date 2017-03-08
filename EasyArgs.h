#ifndef __EASY_ARGS_H
#define __EASY_ARGS_H
#include "uthash.h"

typedef struct CommandLineArgTemplate {
	char *description;
	char *name;
	char *longName;
	int required;
	int takesArg;
	int present;
	char *value; // Filled in by ParseCommandLineArgs
	UT_hash_handle nameHH;
	UT_hash_handle longNameHH;
} CommandLineArgTemplate;

#define CreateArgTemplate(name, longName, required, takesArg, description) \
{ description, name, longName, required, takesArg, 0, NULL }

int ParseCommandLineArgs(int argc, char *argv[], CommandLineArgTemplate *templates[], int templatesCount, CommandLineArgTemplate *configFileOptionTemplate, char *defaultConfigFile, char **outError);

void FreeCommandLineArgTemplateResources(CommandLineArgTemplate *templates[], int templatesCount);

#endif /* EasyArgs.h */
