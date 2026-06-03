#include <stdio.h>
#include <stdlib.h>

extern int ft_atoi_base(char const *str, char const *base);

void test(char *str, char *base, char *expect)
{
	printf("Num: %s, Base: %s | Expected: %s, Res: %d\n", str, base, expect, ft_atoi_base(str, base));
}

 int	main(int ac, char **av)
 {
 	if (ac == 3)
	{
		printf("Given in parameters:\n");
 		test(av[1], av[2], "N/A");
	}

	printf("\nBasic sanity checks.\n");
	test("1", "01", "1");
	test("10", "01", "2");
	test("ff", "0123456789abcdef", "255");
	test("246","01234567", "166");
	test("eric", "0e2r4i6c", "751");

	printf("\nSign tests.\n");
	test("-10", "01", "-2");
	test("--10", "01", "2");
	test("---10", "01", "-2");
	test("+-10", "01", "-2");
	test("-+10", "01", "-2");
	test("  +--10", "01", "2");

	printf("\nEarly number end.\n");
	test("10 11", "01", "2");
	test("10+11", "01", "2");
	test("10-11", "01", "2");
	test("1x1", "01", "1");

	printf("\nInvalid bases.\n");
	test("10", "0", "0");
	test("10", "", "0");
	test("10", "010", "0");
	test("10", "0+1", "0");
	test("10", "0-1", "0");
	test("10", "0 1", "0");

	printf("\nNo valid digits.\n");
	test("", "01", "0");
	test("+", "01", "0");
	test("   ", "01", "0");
	test("abc", "01", "0");

	printf("\nLarge values.\n");
	test("7fffffff", "0123456789abcdef", "2147483647");
	test("80000000", "0123456789abcdef", "-2147483648");

	printf("\nUppercase/lowercase.\n");
	test("FF", "0123456789abcdef", "0");
	test("FF", "0123456789ABCDEF", "255");
	test("Ff", "0123456789abcdef", "0");
	test("fF", "0123456789abcdef", "15");

	// These cause segfault
	//printf("null test: %d\n", ft_atoi_base(NULL, "012345"));
	//printf("null test: %d\n", ft_atoi_base("012345", NULL));
 }
