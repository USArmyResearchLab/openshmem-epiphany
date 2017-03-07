// Fixes for the global labels and underscores
#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b
#define SYM(s) CONCAT1(__USER_LABEL_PREFIX__, s)
#define SIZE(s) .size SYM(s), .-SYM(s)
