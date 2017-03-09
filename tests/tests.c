#include "minunit.h"
#include "EasyArgs.h"
#include <stdio.h>

int tests_run = 0;

static char *testUnkownOption() {
	CommandLineArgTemplate foo = CreateArgTemplate("f", NULL, 0, 0, NULL);
	CommandLineArgTemplate *templates[] = { &foo };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);

	char *argv[] = { "", "-u" };
	int argc = sizeof(argv)/sizeof(argv[0]);
	mu_assert(ParseCommandLineArgs(argc, argv, templates, templatesCount, NULL, NULL, NULL) < 0, "Unknown option should be an error!");
	return 0;
}

static char *testMissingArgument() {
	CommandLineArgTemplate foo = CreateArgTemplate("f", NULL, 0, 1, NULL);
	CommandLineArgTemplate *templates[] = { &foo };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);

	char *argv[] = { "", "-f", "-f" };
	char *message = "Option missing its argument should be an error!";
	mu_assert(ParseCommandLineArgs(2, argv, templates, templatesCount, NULL, NULL, NULL) < 0, message);
	mu_assert(ParseCommandLineArgs(3, argv, templates, templatesCount, NULL, NULL, NULL) < 0, message);

	return 0;
}

static char *testArgument() {
	CommandLineArgTemplate foo = CreateArgTemplate("f", NULL, 0, 1, NULL);
	CommandLineArgTemplate *templates[] = { &foo };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);
	
	char *argv[] = { "", "-f", "HelloWorld" };
	ParseCommandLineArgs(3, argv, templates, templatesCount, NULL, NULL, NULL);
	mu_assert(strcmp(foo.value, "HelloWorld") == 0, "Option argument not parsed!");
	return 0;
}

static char *testOptionPresent() {
	CommandLineArgTemplate foo = CreateArgTemplate("f", NULL, 0, 0, NULL);
	CommandLineArgTemplate *templates[] = { &foo };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);
	
	char *argv[] = { "", "-f" };
	ParseCommandLineArgs(2, argv, templates, templatesCount, NULL, NULL, NULL);
	mu_assert(foo.present == 1, "Option should be marked as present!");
	return 0;
}

static char *testMissingRequiredOption() {
	CommandLineArgTemplate foo = CreateArgTemplate("f", NULL, 1, 0, NULL);
	CommandLineArgTemplate *templates[] = { &foo };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);

	char *argv[] = { "" };
	char *message = "A required option that is missing should be an error!";
	mu_assert(ParseCommandLineArgs(1, argv, templates, templatesCount, NULL, NULL, NULL) < 0, message);

	char *argv2[] = { "", "-g" };
	mu_assert(ParseCommandLineArgs(2, argv2, templates, templatesCount, NULL, NULL, NULL) < 0, message);

	return 0;
}

static char *testDuplicateTemplateArguments() {
	CommandLineArgTemplate foo1 = CreateArgTemplate("f", "foo", 0, 0, NULL);
	CommandLineArgTemplate foo2 = CreateArgTemplate("f", "foo", 0, 0, NULL);
	CommandLineArgTemplate *templates[] = { &foo1, &foo2 };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);

	char *argv[] = { "", "-f" };
	int argc = sizeof(argv)/sizeof(argv[0]);
	
	mu_assert(ParseCommandLineArgs(argc, argv, templates, templatesCount, NULL, NULL, NULL) == 0, "Multiple command line arg templates with the same name or long name should cause the functional to fail out");

	return 0;
}

static char *testReadArgumentsFromDefaultConfig() {
	CommandLineArgTemplate foo = CreateArgTemplate("f", NULL, 0, 1, NULL);
	CommandLineArgTemplate *templates[] = { &foo };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);

	char *argv[] = { "" };

	ParseCommandLineArgs(1, argv, templates, templatesCount, NULL, "test.conf", NULL);
	mu_assert(foo.value != NULL && strcmp(foo.value, "HelloWorld") == 0, "Command line arg value should be read from 'testconfig' file");
	return 0;
}

static char *testDefaultConfigArgumentOverride() {
	CommandLineArgTemplate foo = CreateArgTemplate("f", NULL, 0, 1, NULL);
	CommandLineArgTemplate *templates[] = { &foo };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);

	char *argv[] = { "", "-f", "NotHelloWorld" };

	ParseCommandLineArgs(3, argv, templates, templatesCount, NULL, "test.conf", NULL);
	mu_assert(foo.value != NULL && strcmp(foo.value, "NotHelloWorld") == 0, "Command line arg value should be read from 'testconfig' file");
	return 0;
}

static char *testConfigOption() {
	CommandLineArgTemplate foo = CreateArgTemplate("f", NULL, 0, 1, NULL);
	CommandLineArgTemplate config = CreateArgTemplate("c", NULL, 0, 1, NULL);
	CommandLineArgTemplate *templates[] = { &foo, &config };
	int templatesCount = sizeof(templates)/sizeof(templates[0]);

	char *argv[] = { "", "-c", "test.conf" };

	ParseCommandLineArgs(3, argv, templates, templatesCount, &config, "test.conf", NULL);
	mu_assert(foo.value != NULL && strcmp(foo.value, "HelloWorld") == 0, "Command line arg value should be read from 'testconfig' file");
	return 0;
}


static char *allTests() {
	mu_run_test(testUnkownOption);
	mu_run_test(testMissingArgument);
	mu_run_test(testMissingRequiredOption);
	mu_run_test(testOptionPresent);
	mu_run_test(testDuplicateTemplateArguments);
	mu_run_test(testReadArgumentsFromDefaultConfig);
	mu_run_test(testDefaultConfigArgumentOverride);
	mu_run_test(testConfigOption);
	mu_run_test(testArgument);
	return 0;
}

int main(int argc, char **argv) {
     char *result = allTests();
     if (result != 0) {
         printf("%s\n", result);
     }
     else {
         printf("ALL TESTS PASSED\n");
     }
     printf("Tests run: %d\n", tests_run);

     return result != 0;
}
			
