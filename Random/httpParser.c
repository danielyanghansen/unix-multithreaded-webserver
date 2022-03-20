#include <stdio.h>
#include <string.h>

int parse(const char* line) //Tried to implement this into the main server. Will try again later.
{
    /* Find out where everything is */
    const char *start_of_path = strchr(line, ' ') + 1;
    const char *start_of_query = strchr(start_of_path, '?');
    const char *end_of_query = strchr(start_of_query, ' ');

    /* Get the right amount of memory */
    char path[start_of_query - start_of_path];
    char query[end_of_query - start_of_query];
    char method[start_of_path - &line[0]];

    /* Copy the strings into our memory */
    strncpy(path, start_of_path,  start_of_query - start_of_path);
    strncpy(query, start_of_query, end_of_query - start_of_query);
    strncpy(method, &line[0], start_of_path-&line[0]);

    /* Null terminators (because strncpy does not provide them) */
    path[sizeof(path)] = 0;
    query[sizeof(query)] = 0;
    method[sizeof(method)] = '\0'; 

    /*Print */
    printf("Request: %s\n\n", line);

    printf("Method: %s\n", method, (10));
    printf("Sizeof method: %i\n", (int) sizeof(method));
    printf("%s\n", path, sizeof(path));
    printf("%s\n", query, sizeof(query));

    return 0; 
}

int main(void)
{
    parse("GET /path/script.cgi?field1=value1&field2=value2 HTTP/1.1");
    return 0;
}