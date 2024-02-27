#ifndef TEST_ASSERTIONS_H
#define TEST_ASSERTIONS_H

#define arraylen(x) (sizeof(x) / sizeof(x[0]))

#define EXPECT_ARRAY_EQ(lhs, rhs)                                               \
	if (arraylen(lhs) != arraylen(rhs)) {                                       \
		if (rktest_filenames_enabled()) {                                       \
			printf("%s(%d): ", __FILE__, __LINE__);                             \
		}                                                                       \
		printf("error: Arrays do not match in length!\n");                      \
		printf("  '%s'\n    Has length: %lld\n", #lhs, arraylen(lhs));          \
		printf("  '%s'\n    Has length: %lld\n", #rhs, arraylen(rhs));          \
		rktest_fail_current_test();                                             \
	}                                                                           \
	for (int i = 0; i < arraylen(lhs) && arraylen(lhs) == arraylen(rhs); i++) { \
		EXPECT_EQ_INFO(lhs[i], rhs[i], "i = %d\n", i);                          \
	}

#define EXPECT_CHAR_ARRAY_EQ(lhs, rhs)                                          \
	if (arraylen(lhs) != arraylen(rhs)) {                                       \
		if (rktest_filenames_enabled()) {                                       \
			printf("%s(%d): ", __FILE__, __LINE__);                             \
		}                                                                       \
		printf("error: Arrays do not match in length!\n");                      \
		printf("  '%s'\n    Has length: %lld\n", #lhs, arraylen(lhs));          \
		printf("  '%s'\n    Has length: %lld\n", #rhs, arraylen(rhs));          \
		rktest_fail_current_test();                                             \
	}                                                                           \
	for (int i = 0; i < arraylen(lhs) && arraylen(lhs) == arraylen(rhs); i++) { \
		EXPECT_CHAR_EQ_INFO(lhs[i], rhs[i], "i = %d\n", i);                     \
	}

#endif /* TEST_ASSERTIONS_H */
