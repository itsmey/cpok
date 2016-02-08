#ifndef TESTS_H
#define TESTS_H

#define TEST(Name, ExecFunction, ExpectedValue) \
  printf(Name); \
  if (ExecFunction == ExpectedValue) printf("passed: "); else printf ("failed: "); \
  printf(#ExecFunction" = "#ExpectedValue"\n");

void test(void);

#endif
