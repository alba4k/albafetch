#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// We need to include the actual header to call getLogoLine
#include "src/utils/utils.h"

// Mock config structure needed by getLogoLine
extern struct Config config;

START_TEST(test_logo_buffer_bounds)
{
    // Invariant: Destination buffer must not overflow regardless of logo content
    const char *payloads[] = {
        "A",  // Minimal valid input
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",  // 255 chars
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",  // 256 chars (max line length)
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",  // 257 chars (overflow by 1)
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        // Setup config.logo with adversarial line
        char *logo_lines[] = {NULL, NULL, NULL, NULL};
        logo_lines[1] = (char*)payloads[i];  // Line 1 is the first logo line
        config.logo = logo_lines;
        
        // Create destination buffer with canary values to detect overflow
        #define DEST_SIZE 512
        char dest[DEST_SIZE];
        memset(dest, 0xAA, DEST_SIZE);  // Fill with canary pattern
        dest[0] = '\0';  // Start with empty string for strcat
        dest[DEST_SIZE-1] = '\0';  // Ensure null-terminated
        
        unsigned line = 0;
        
        // Call the actual production function
        getLogoLine(dest, &line);
        
        // Security property: canary bytes after buffer must remain unchanged
        // We check a few bytes beyond what should be the maximum used space
        int max_expected_length = 256;  // Max line length
        for (int j = max_expected_length + 10; j < DEST_SIZE - 1; j++) {
            ck_assert_msg(dest[j] == (char)0xAA, 
                         "Buffer overflow detected at position %d (expected 0xAA, got 0x%02x)",
                         j, dest[j] & 0xFF);
        }
        
        // Also ensure the string remains null-terminated within bounds
        ck_assert_msg(strnlen(dest, DEST_SIZE) < DEST_SIZE,
                     "String not properly null-terminated within buffer");
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_logo_buffer_bounds);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}