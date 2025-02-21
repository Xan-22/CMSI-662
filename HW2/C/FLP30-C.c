#include <stdio.h>
void noncompliant(void) { // THIS IS NON-COMPLIANT CODE
    float x = 0.1f;
    while (x <= 1.0f) {
        printf("x: %f\n", x);
        x += 0.1f; // May loop 9 or 10 times
    }
}

void compliant(void) { // THIS IS COMPLIANT CODE
    int i = 1;
    while (i <= 10) {
        printf("x: %f\n", (float)(i * 0.1f));
        i++; // Will always loop 10 times
    }
}

int main(void) {
    noncompliant();
    puts("----");
    compliant();
    return 0;
}