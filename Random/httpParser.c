#include <stdio.h>
#include <string.h>

int parse(const char* line)
{
    /* Find out where everything is */
    const char *start_of_path = strchr(line, ' ') + 1;
    const char *start_of_query = strchr(start_of_path, '?');
    const char *end_of_query = strchr(start_of_query, ' ');

    /* Get the right amount of memory */
    char path[start_of_query - start_of_path];
    char query[end_of_query - start_of_query];

    /* Copy the strings into our memory */
    strncpy(path, start_of_path,  start_of_query - start_of_path);
    strncpy(query, start_of_query, end_of_query - start_of_query);

    /* Null terminators (because strncpy does not provide them) */
    path[sizeof(path)] = 0;
    query[sizeof(query)] = 0;

    /*Print */
    printf("%s\n", query, sizeof(query));
    printf("%s\n", path, sizeof(path));
}

int main(void)
{
    parse("GET /path/script.cgi?field1=value1&field2=value2 HTTP/1.1");
    return 0;
}