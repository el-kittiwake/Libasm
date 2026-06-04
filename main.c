#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

char	singleAct[2];
char	singleExp[2];

extern size_t ft_strlen(const char *str);
extern char *ft_strcpy(char *dest, const char *src);
extern int ft_strcmp(const char *s1, const char *s2);
extern ssize_t ft_write(int fd, const void *buf, size_t count);
extern ssize_t ft_read(int fd, void *buf, size_t count);
extern char *ft_strdup(const char *s);

void ft_strlen_test(char *str)
{
	printf("Str: %s | Len: %ld\n", str, strlen(str));
	if (strlen(str) != ft_strlen(str))
		printf("libc: %ld |libasm: %ld - WRONG.\n",strlen(str), ft_strlen(str));
}

void ft_strcpy_test(char *string)
{
	char	actual[100];
	char	expected[100];

	ft_strcpy(actual, string);
	strcpy(expected, string);
	printf("libc: %s | libasm: %s\n", expected, actual);
	if (strcmp(expected, actual) != 0)
		printf("WRONG.\n");
}

void ft_strcmp_test(char *a1, char *b1)
{
	printf("String 1: %s\n", a1);
	printf("String 2: %s\n", b1);

	int orig_ret = strcmp(a1, b1);
	int ret = ft_strcmp(a1, b1);
	printf("libc: %d | libasm: %d\n", orig_ret, ret);
	if (orig_ret != ret)
		printf("WRONG.\n");
}

void ft_write_test(int fd, const void *buf, size_t count)
{
	errno = 0;
	ssize_t ft_result = ft_write(fd, buf, count);
	int ft_errno = errno;
	errno = 0;
	ssize_t libc_result = write(fd, buf, count);
	int libc_errno = errno;

	printf("libc: written: %ld, %s | libasm: written: %ld, %s\n", libc_result,
		strerror(libc_errno), ft_result, strerror(ft_errno));
	if (!((ft_result == libc_result) && (ft_errno == libc_errno)))
		printf("WRONG.\n");
}

#define READ_TEST_FILE "/tmp/ft_read_test.txt"

void ft_read_test(int ft_fd, int libc_fd, void *buf, size_t count)
{
    errno = 0;
    ssize_t ft_result = ft_read(ft_fd, buf, count);
    int ft_errno = errno;
    errno = 0;
    ssize_t libc_result = read(libc_fd, buf, count);
    int libc_errno = errno;

    printf("libc: read: %ld, %s | libasm: read: %ld, %s\n", libc_result,
        strerror(libc_errno), ft_result, strerror(ft_errno));
    if (!((ft_result == libc_result) && (ft_errno == libc_errno)))
        printf("WRONG.\n");
}

void ft_read_file_test(size_t count)
{
    void *buf[100];
    int fd_ft   = open(READ_TEST_FILE, O_RDONLY);
    int fd_libc = open(READ_TEST_FILE, O_RDONLY);
    ft_read_test(fd_ft, fd_libc, buf, count);
    close(fd_ft);
    close(fd_libc);
}

void ft_strdup_test(char *string)
{
	char *ft_out = ft_strdup(string);
	if (ft_out == NULL)
	{
		printf("ft_strdup returned NULL, errno: %d (%s)\n", errno, strerror(errno));
		free(ft_out);
		return ;
	}
	char *libc_out = strdup(string);
	printf("libc: %s | libasm: %s\n", libc_out, ft_out);
	if (strcmp(libc_out, ft_out) != 0)
		printf("WRONG.\n");
	free(ft_out);
	free(libc_out);
}


int main(void)
{
	{
	printf("============================\n===== ft_strlen test group.\n===========================\n");
	ft_strlen_test("Blue tit");
	ft_strlen_test("69");
	ft_strlen_test("\t\n\n\f\r\n\t");
	ft_strlen_test("            ");
	ft_strlen_test("");
	ft_strlen_test("The\tquick\tbrown\nfox\tjumps\nover\nthe lazy\t dog\n");
	ft_strlen_test("I opened the door only to see \x42, what was she doing here\e?");
	ft_strlen_test("♫♪.ılılıll|̲̅̅●̲̅̅|̲̅̅=̲̅̅|̲̅̅●̲̅̅|llılılı.♫♪");
	ft_strlen_test("Harry \0 Belafonte is a great guy.");

	//Big buffer test from libftunittests.
	int		r1, r2, offset, rnd_fd = open("/dev/urandom", O_RDONLY);
	char	buff[0x2000];
	const int test_count = 100;
	if (rnd_fd < 0){
		printf("Something unrelated to libasm went wrong.\n");
		return 1;
	}
	for (int i = 0; i < test_count; i++)
	{
		if (read(rnd_fd, buff, sizeof(buff)) > 0)
		{
			offset = rand() % sizeof(buff);
			buff[sizeof(buff) - 1] = 0;
			if ((r1 = ft_strlen(buff + offset)) != (r2 = strlen(buff + offset))) 
				printf("libc: %d | libasm: %d - WRONG\n", r2, r1);
		}
	}
	printf("Random buffer ran: %d times. Success.\n\n", test_count);
	}


	{
	printf("============================\n===== ft_strcpy test group.\n============================\n");
	ft_strcpy_test("Blue tit");
	ft_strcpy_test("69");
	ft_strcpy_test("\t\n\n\f\r\n\t");
	ft_strcpy_test("            ");
	ft_strcpy_test("");
	ft_strcpy_test("The\tquick\tbrown\nfox\tjumps\nover\nthe lazy\t dog\n");
	ft_strcpy_test("I opened the door only to see \x42, what was she doing here\e?");
	ft_strcpy_test("♫♪.ılılıll|̲̅̅●̲̅̅|̲̅̅=̲̅̅|̲̅̅●̲̅̅|llılılı.♫♪");
	ft_strcpy_test("Harry \0 Belafonte is a great guy.");

	printf("\n== Buffer length string.\n");
	ft_strcpy_test("LOREM ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. AENEAN M");

	printf("\n== Single char, single char buffer\n");
	ft_strcpy(singleAct, "d");
	strcpy(singleExp, "d");
	printf("libc: %s | libasm: %s | Match: %s\n\n", singleExp, singleAct, strcmp(singleExp, singleAct) == 0 ? "yes" : "no");

	// Comment/uncomment to do crash tests.
/*
	printf("\nVarious undefined things, may crash after this point:\n");
	char tiny[3];
	ft_strcpy(tiny, "This string is far too long for the buffer that I am trying to copy it to, it may or may not display properly at run time. It may or may not cause a crash.");
	printf("Writing long string to short buffer. Potential stack smashing:\n");
	printf("String: %s | Expected: %s\n\n", tiny, "This string is far too long for the buffer that I am trying to copy it to, it may or may not display properly at run time. It may or may not cause a crash.");
*/

	//printf("\nReading and writing to NULL, these will segfault:\n");
	//ft_strcpy(NULL, "hello");	// writing to NULL
	//ft_strcpy(actual, NULL);	// reading from NULL
	}


	{
	printf("============================\n===== ft_strcmp test group.\n============================\n");
	ft_strcmp_test("Blue tit", "Blue tit");
	ft_strcmp_test("Blue tit", "Vlue tis");
	ft_strcmp_test("Vlue tis", "Blue tit");
	ft_strcmp_test("Blue tit", "Blue bit");
	ft_strcmp_test("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaac");
	printf("\n== Empty strings.\n");
	ft_strcmp_test("","");
	ft_strcmp_test("Full","");
	ft_strcmp_test("","Full");
	printf("\n== Identical string, different address.\n");
	char a[] = "identical";
	char b[] = "identical";
	ft_strcmp_test(a, b);
	printf("\n== Single char strings.\n");
	ft_strcmp_test("a", "a");
	ft_strcmp_test("b", "a");
	ft_strcmp_test("a", "");
	printf("\n== Different length strings.\n");
	ft_strcmp_test("Crossbill", "Cross");
	ft_strcmp_test("Cross", "Crossbill");
	printf("\n== Strange character strings.\n");
	char *src4 = "Jackdaws \0love my\0\0 big\0\0\0 sphynx \0\0\0\0of \0\0\0\0\0quartz.";
	char *src5 = "Jackdaws \0Love my\0\0 big\0\0\0 sphynx \0\0\0\0of \0\0\0\0\0quartz.";
	ft_strcmp_test(src4, src5);
	ft_strcmp_test("\0", "\0");
	ft_strcmp_test("\200", "\0");
	ft_strcmp_test("\x12\xff\x65\x12\xbd\xde\xad", "\x12\x02");
	ft_strcmp_test("\x12\x02", "\x12\xff\x65\x12\xbd\xde\xad");
	ft_strcmp_test("\001\002\003\004\005\006\a\b\t\n\v\f\016\017\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037 !\042#$%&'()*+,-./0123456789:;M<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\140abcdefghijklmnopqrstuvwxyz{|]~", "\001\002\003\004\005\006\a\b\t\n\v\f\016\017\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037 !\042#$%&'()*+,-./0123456789:;M<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\140abcdefghijklmnopqrstuvwxyz{|]~");
	ft_strcmp_test("\x80", "\x01");
	ft_strcmp_test("\x01", "\x80");
	ft_strcmp_test("\xff", "\xfe");
	ft_strcmp_test("\xfe", "\xff");

	//printf("NULL sent as parameter(s). Likely segfault.\n");
	//ft_strcmp(NULL, src1);
	//ft_strcmp(src1, NULL);
	}


	{
	printf("\n============================\n===== ft_write test group.\n============================\n");
	printf("== Valid write to stdout\n");
	char buffer[100] = "Hello, World!\n";
	ssize_t bytes_written;
	bytes_written = ft_write(STDOUT_FILENO, buffer, strlen(buffer));
	if (bytes_written == -1) {
		perror("ft_write");
		return 1;
	}
	printf("Bytes written: %zd\n", bytes_written);

	char *buf = "hello\n";
	printf("== Invalid fd\n");
	ft_write_test(-1, buf, 5);
	printf("== fd out of range\n");
	ft_write_test(99999, buf, 5);
	printf("== NULL buffer\n");
	ft_write_test(1, NULL, 5);
	printf("== Zero count (valid)\n");
	ft_write_test(1, buf, 0);
	printf("== Valid write to stderr\n");
	ft_write_test(2, buf, 6);

	printf("== Write to closed fd\n");
	int fds[2];
	pipe(fds);
	close(fds[0]);
	close(fds[1]);
	ft_write_test(fds[1], buf, 5);
	}


	{
	printf("\n============================\n===== ft_read test group.\n============================\n");
	char *buf[100];
	printf("== Invalid fd\n");
	ft_read_test(-1, -1, buf, 5);
	printf("== fd out of range\n");
	ft_read_test(99999, 99999, buf, 5);
	printf("== Zero count (valid)\n");
	ft_read_test(1, 1, buf, 0);

	printf("== Read from closed fd\n");
	int fds[2];
	pipe(fds);
	close(fds[0]);
	close(fds[1]);
	ft_read_test(fds[1], fds[1], buf, 5);

	printf("\n== Read from /dev/null (expect 0 bytes)\n");
	int null_fd = open("/dev/null", O_RDONLY);
	ft_read_test(null_fd, null_fd, buf, 10);
	close(null_fd);

	printf("== Read exact file size\n");
	ft_read_file_test(5);
	
	printf("== Read less than available\n");
	ft_read_file_test(3);

	printf("== Request more than available\n");
	ft_read_file_test(100);

	printf("== Read at EOF (expect 0)\n");
	int fd7 = open("/tmp/ft_read_test.txt", O_RDONLY);
	int fd8 = open("/tmp/ft_read_test.txt", O_RDONLY);
	lseek(fd7, 0, SEEK_END);
	lseek(fd8, 0, SEEK_END);
	ft_read_test(fd7, fd8, buf, 5);
	close(fd7);
	close(fd8);

	printf("== Read from pipe with data\n");
	int p1[2], p2[2];
	pipe(p1); pipe(p2);
	write(p1[1], "abc", 3); close(p1[1]);
	write(p2[1], "abc", 3); close(p2[1]);
	ft_read_test(p1[0], p2[0], buf, 3);
	close(p1[0]); close(p2[0]);

	//printf("== NULL buffer. Will hang. Turn off CFLAGS.\n");
	//ft_read_test(1, NULL, 5);
	//read(1, NULL, 5);
	}


	{
	printf("\n============================\n===== ft_strdup test group.\n============================\n");
	ft_strdup_test("Jackdaws love my big sphynx of quartz.");
	ft_strdup_test("A");
	ft_strdup_test("1! 2?");
	ft_strdup_test("The secret is:\0The next string is empty!");
	ft_strdup_test("");
	ft_strdup_test("LOREM ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est LABORUM.");

	//printf("The following will cause segfault.\n");
	//write(1, ft_strdup(NULL), 2);
	//write(1, strdup(NULL), 2);
	}
	
	printf("All tests complete.\n");
}