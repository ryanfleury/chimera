#define debug_log(f, ...)  fprintf(stdout, "ERROR: " f "\n", __VA_ARGS__)
#define error(f, ...)      fprintf(stderr, "ERROR: " f "\n", __VA_ARGS__)
#define assert_statement(statement) if(!(statement)) _assert_fail_(__FILE__, __LINE__, #statement);

void _assert_fail_(const char *file, i32 line, 
                   const char *failed_statement) {
    error("Assertion \"%s\" failed in \"%s\" on line %i",
          failed_statement, file, line);
    debug_log("Trying to exit cleanly...");
    exit(0);
}
