/*
 * tests/unit/test_shell.c
 *
 * Unit tests for the pure string helpers declared in includes/shell.h and
 * defined in source/shell.c:
 *
 * 
 */

#include "unity.h"
#include "shell.h"

#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* Free any strdup'd tokens left in a command array, so each test is
 * self-contained and does not leak. */
static void free_cmd(char **cmd)
{
    for (int i = 0; cmd[i] != NULL; i++) {
        free(cmd[i]);
        cmd[i] = NULL;
    }
}

/* ---- split_command: normal cases ---- */

static void test_split_single_command(void)
{
    char *cmd[MAX_ARGS];
    char line[] = "ls";
    split_command(cmd, line);

    TEST_ASSERT_NOT_NULL(cmd[0]);
    TEST_ASSERT_EQUAL_STRING("ls", cmd[0]);
    TEST_ASSERT_NULL(cmd[1]);

    free_cmd(cmd);
}

static void test_split_command_with_arguments(void)
{
    char *cmd[MAX_ARGS];
    char line[] = "ls -l -a";
    split_command(cmd, line);

    TEST_ASSERT_EQUAL_STRING("ls", cmd[0]);
    TEST_ASSERT_EQUAL_STRING("-l", cmd[1]);
    TEST_ASSERT_EQUAL_STRING("-a", cmd[2]);
    TEST_ASSERT_NULL(cmd[3]);

    free_cmd(cmd);
}

/* ---- split_command: edge cases ---- */

static void test_split_collapses_repeated_and_outer_spaces(void)
{
    /* strtok treats runs of delimiters as one, and ignores leading/trailing
     * delimiters, so the token list has no empty entries. */
    char *cmd[MAX_ARGS];
    char line[] = "   ls    -l   ";
    split_command(cmd, line);

    TEST_ASSERT_EQUAL_STRING("ls", cmd[0]);
    TEST_ASSERT_EQUAL_STRING("-l", cmd[1]);
    TEST_ASSERT_NULL(cmd[2]);

    free_cmd(cmd);
}

static void test_split_strips_trailing_newline(void)
{
    /* '\n' is one of the delimiters (" \n"), so a trailing newline does not
     * produce an extra token and is not part of the last token. */
    char *cmd[MAX_ARGS];
    char line[] = "echo hi\n";
    split_command(cmd, line);

    TEST_ASSERT_EQUAL_STRING("echo", cmd[0]);
    TEST_ASSERT_EQUAL_STRING("hi", cmd[1]);
    TEST_ASSERT_NULL(cmd[2]);

    free_cmd(cmd);
}

static void test_split_empty_line_yields_no_tokens(void)
{
    char *cmd[MAX_ARGS];
    char line[] = "";
    split_command(cmd, line);

    TEST_ASSERT_NULL(cmd[0]);
}

static void test_split_whitespace_only_yields_no_tokens(void)
{
    char *cmd[MAX_ARGS];
    char line[] = "   \n";
    split_command(cmd, line);

    TEST_ASSERT_NULL(cmd[0]);
}

static void test_split_tab_is_not_a_delimiter(void)
{
    /* The delimiter set is exactly " \n" (space and newline); a tab is kept
     * as part of the token. This documents the current behaviour. */
    char *cmd[MAX_ARGS];
    char line[] = "a\tb";
    split_command(cmd, line);

    TEST_ASSERT_EQUAL_STRING("a\tb", cmd[0]);
    TEST_ASSERT_NULL(cmd[1]);

    free_cmd(cmd);
}

/* ---- clean_arr ---- */

static void test_clean_arr_nulls_out_populated_array(void)
{
    char *cmd[MAX_ARGS];
    char line[] = "one two three";
    split_command(cmd, line);
    TEST_ASSERT_NOT_NULL(cmd[0]); /* precondition: array is populated */

    clean_arr(cmd);

    TEST_ASSERT_NULL(cmd[0]);
    TEST_ASSERT_NULL(cmd[1]);
    TEST_ASSERT_NULL(cmd[2]);
}

static void test_clean_arr_on_empty_array_is_noop(void)
{
    char *cmd[MAX_ARGS];
    cmd[0] = NULL;

    clean_arr(cmd); /* must not crash on an already-empty array */

    TEST_ASSERT_NULL(cmd[0]);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_split_single_command);
    RUN_TEST(test_split_command_with_arguments);
    RUN_TEST(test_split_collapses_repeated_and_outer_spaces);
    RUN_TEST(test_split_strips_trailing_newline);
    RUN_TEST(test_split_empty_line_yields_no_tokens);
    RUN_TEST(test_split_whitespace_only_yields_no_tokens);
    RUN_TEST(test_split_tab_is_not_a_delimiter);
    RUN_TEST(test_clean_arr_nulls_out_populated_array);
    RUN_TEST(test_clean_arr_on_empty_array_is_noop);
    return UNITY_END();
}
