#define assert_statement(statement) if(!(statement)) _assert_fail_(__FILE__, __LINE__, #statement);

void debug_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
}

void error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void _assert_fail_(const char *file, i32 line, 
                   const char *failed_statement) {
    
    error("Assertion \"%s\" failed in \"%s\" on line %i",
          failed_statement, file, line);
    
    debug_log("Trying to exit cleanly...");
    exit(0);
}
