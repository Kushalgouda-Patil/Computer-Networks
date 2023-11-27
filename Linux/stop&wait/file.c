#include <stdio.h>

int main() {
    FILE *file_ptr;
    char buffer[5];

    file_ptr = fopen("example.txt", "r");
    
    if (file_ptr == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Reading content using fgets
    while (fgets(buffer, sizeof(buffer)-1, file_ptr) != NULL) {
        buffer[sizeof(buffer)-1]=0; 
        printf("%s$", buffer);
    }

    fclose(file_ptr);

    return 0;
}
