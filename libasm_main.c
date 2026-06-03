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
extern int ft_atoi_base(const char *str, const char *base);

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

void ft_atoi_base_test(char *str, char *base, char *expect)
{
	printf("Num: %s, Base: %s | Expected: %s, Res: %d\n", str, base, expect, ft_atoi_base(str, base));
}

static const char *ft_read_errno_str(int err)
{
	return err ? strerror(err) : "none";
}

static void ft_read_result(const char *test, int passed, const char *note)
{
	if (passed)
		printf("  [PASS] %s\n", test);
	else
		printf("  [FAIL] %s — %s\n", test, note ? note : "");
}

static void ft_read_print(const char *label,
                          ssize_t ft_ret, int ft_errno, const char *ft_buf,
                          ssize_t libc_ret, int libc_errno, const char *libc_buf)
{
	printf("  [%s]\n", label);
	printf("    ft_read: return=%zd errno=%d (%s)\n",
		   ft_ret, ft_errno, ft_read_errno_str(ft_errno));
	if (ft_buf)
		printf("             buf=\"%.*s\"\n",
		       (int)(ft_ret > 0 ? ft_ret : 0), ft_buf);
	printf("    read   : return=%zd errno=%d (%s)\n",
		   libc_ret, libc_errno, ft_read_errno_str(libc_errno));
	if (libc_buf)
		printf("             buf=\"%.*s\"\n",
		       (int)(libc_ret > 0 ? libc_ret : 0), libc_buf);
}

static int ft_read_make_tmpfile(const char *content)
{
	int fd;
	char path[] = "/tmp/ft_read_test_XXXXXX";

	fd = mkstemp(path);
	if (fd < 0)
	{
		perror("mkstemp");
		exit(1);
	}
	unlink(path);
	write(fd, content, strlen(content));
	lseek(fd, 0, SEEK_SET);
	return fd;
}

static void ft_read_test_basic(void)
{
	const char *content = "Hello, world!";
	char ft_buf[64] = {0};
	char libc_buf[64] = {0};
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int fd1, fd2;
	char note[128];

	fd1 = ft_read_make_tmpfile(content);
	fd2 = ft_read_make_tmpfile(content);

	errno = 0;
	ft_ret = ft_read(fd1, ft_buf, 13);
	ft_errno = errno;

	errno = 0;
	libc_ret = read(fd2, libc_buf, 13);
	libc_errno = errno;

	ft_read_print("basic read", ft_ret, ft_errno, ft_buf, libc_ret, libc_errno, libc_buf);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("basic read — return value matches", ft_ret == libc_ret, note);
	ft_read_result("basic read — buffer content matches",
	               memcmp(ft_buf, libc_buf, 13) == 0,
	               "buffers differ");

	close(fd1);
	close(fd2);
}

static void ft_read_test_partial(void)
{
	const char *content = "ABCDEFGHIJ";
	char ft_buf[64] = {0};
	char libc_buf[64] = {0};
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int fd1, fd2;
	char note[128];

	fd1 = ft_read_make_tmpfile(content);
	fd2 = ft_read_make_tmpfile(content);

	errno = 0;
	ft_ret = ft_read(fd1, ft_buf, 4);
	ft_errno = errno;

	errno = 0;
	libc_ret = read(fd2, libc_buf, 4);
	libc_errno = errno;

	ft_read_print("partial read", ft_ret, ft_errno, ft_buf, libc_ret, libc_errno, libc_buf);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("partial read — return value matches", ft_ret == libc_ret, note);
	ft_read_result("partial read — buffer content matches",
	               memcmp(ft_buf, libc_buf, 4) == 0,
	               "buffers differ");

	close(fd1);
	close(fd2);
}

static void ft_read_test_zero_bytes(void)
{
	const char *content = "some data";
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int fd1, fd2;
	char note[128];

	fd1 = ft_read_make_tmpfile(content);
	fd2 = ft_read_make_tmpfile(content);

	errno = 0;
	ft_ret = ft_read(fd1, NULL, 0);
	ft_errno = errno;

	errno = 0;
	libc_ret = read(fd2, NULL, 0);
	libc_errno = errno;

	ft_read_print("zero-byte read", ft_ret, ft_errno, NULL, libc_ret, libc_errno, NULL);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("zero-byte read — return value matches", ft_ret == libc_ret, note);

	close(fd1);
	close(fd2);
}

static void ft_read_test_eof(void)
{
	const char *content = "hi";
	char buf[64] = {0};
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int fd1, fd2;
	char note[128];

	fd1 = ft_read_make_tmpfile(content);
	fd2 = ft_read_make_tmpfile(content);

	read(fd1, buf, 64);
	read(fd2, buf, 64);

	errno = 0;
	ft_ret = ft_read(fd1, buf, 64);
	ft_errno = errno;

	errno = 0;
	libc_ret = read(fd2, buf, 64);
	libc_errno = errno;

	ft_read_print("EOF read", ft_ret, ft_errno, NULL, libc_ret, libc_errno, NULL);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("EOF read — return value matches", ft_ret == libc_ret, note);

	close(fd1);
	close(fd2);
}

static void ft_read_test_invalid_fd(void)
{
	char buf[64] = {0};
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	char note[128];

	errno = 0;
	libc_ret = read(-1, buf, 64);
	libc_errno = errno;

	errno = 0;
	ft_ret = ft_read(-1, buf, 64);
	ft_errno = errno;

	ft_read_print("invalid fd", ft_ret, ft_errno, NULL, libc_ret, libc_errno, NULL);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("invalid fd — return value matches libc", ft_ret == libc_ret, note);
	ft_read_result("invalid fd — errno matches libc", ft_errno == libc_errno, note);
}

static void ft_read_test_null_buffer(void)
{
	const char *content = "data";
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int fd1, fd2;
	char note[128];

	fd1 = ft_read_make_tmpfile(content);
	fd2 = ft_read_make_tmpfile(content);

	errno = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
	libc_ret = read(fd2, NULL, 4);
#pragma GCC diagnostic pop
	libc_errno = errno;

	errno = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
	ft_ret = ft_read(fd1, NULL, 4);
#pragma GCC diagnostic pop
	ft_errno = errno;

	ft_read_print("NULL buffer", ft_ret, ft_errno, NULL, libc_ret, libc_errno, NULL);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("NULL buffer — return value matches libc", ft_ret == libc_ret, note);
	ft_read_result("NULL buffer — errno matches libc", ft_errno == libc_errno, note);

	close(fd1);
	close(fd2);
}

static void ft_read_test_null_buffer_zero_count(void)
{
	const char *content = "data";
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int fd1, fd2;
	char note[128];

	fd1 = ft_read_make_tmpfile(content);
	fd2 = ft_read_make_tmpfile(content);

	errno = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
	ft_ret = ft_read(fd1, NULL, 0);
#pragma GCC diagnostic pop
	ft_errno = errno;

	errno = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
	libc_ret = read(fd2, NULL, 0);
#pragma GCC diagnostic pop
	libc_errno = errno;

	ft_read_print("NULL buffer, zero count", ft_ret, ft_errno, NULL, libc_ret, libc_errno, NULL);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("NULL buffer, zero count — return value matches libc", ft_ret == libc_ret, note);
	ft_read_result("NULL buffer, zero count — errno matches libc", ft_errno == libc_errno, note);

	close(fd1);
	close(fd2);
}

static void ft_read_test_write_only_fd(void)
{
	char buf[64] = {0};
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int fd1, fd2;
	char note[128];
	char path1[] = "/tmp/ft_read_wo1_XXXXXX";
	char path2[] = "/tmp/ft_read_wo2_XXXXXX";

	fd1 = mkstemp(path1);
	if (fd1 >= 0)
		close(fd1);
	fd2 = mkstemp(path2);
	if (fd2 >= 0)
		close(fd2);

	fd1 = open(path1, O_WRONLY);
	fd2 = open(path2, O_WRONLY);

	errno = 0;
	libc_ret = read(fd2, buf, 4);
	libc_errno = errno;

	errno = 0;
	ft_ret = ft_read(fd1, buf, 4);
	ft_errno = errno;

	ft_read_print("write-only fd", ft_ret, ft_errno, NULL, libc_ret, libc_errno, NULL);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("write-only fd — return value matches libc", ft_ret == libc_ret, note);
	ft_read_result("write-only fd — errno matches libc", ft_errno == libc_errno, note);

	if (fd1 >= 0) close(fd1);
	if (fd2 >= 0) close(fd2);
	unlink(path1);
	unlink(path2);
}

static void ft_read_test_pipe(void)
{
	char ft_buf[64] = {0};
	char libc_buf[64] = {0};
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int pipe_ft[2], pipe_libc[2];
	char note[128];

	pipe(pipe_ft);
	pipe(pipe_libc);

	write(pipe_ft[1], "pipe data", 9);
	write(pipe_libc[1], "pipe data", 9);
	close(pipe_ft[1]);
	close(pipe_libc[1]);

	errno = 0;
	ft_ret = ft_read(pipe_ft[0], ft_buf, 9);
	ft_errno = errno;

	errno = 0;
	libc_ret = read(pipe_libc[0], libc_buf, 9);
	libc_errno = errno;

	ft_read_print("pipe read", ft_ret, ft_errno, ft_buf, libc_ret, libc_errno, libc_buf);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("pipe read — return value matches", ft_ret == libc_ret, note);
	ft_read_result("pipe read — buffer content matches",
	               memcmp(ft_buf, libc_buf, 9) == 0,
	               "buffers differ");

	close(pipe_ft[0]);
	close(pipe_libc[0]);
}

static void ft_read_test_directory(void)
{
	char buf[64] = {0};
	ssize_t ft_ret, libc_ret;
	int ft_errno, libc_errno;
	int fd1, fd2;
	char note[128];

	fd1 = open(".", O_RDONLY);
	fd2 = open(".", O_RDONLY);
	if (fd1 < 0 || fd2 < 0)
	{
		snprintf(note, sizeof(note), "open failed: %s", strerror(errno));
		printf("  [FAIL] directory fd — open failed — %s\n", note);
		if (fd1 >= 0) close(fd1);
		if (fd2 >= 0) close(fd2);
		return;
	}

	errno = 0;
	ft_ret = ft_read(fd1, buf, sizeof(buf));
	ft_errno = errno;

	errno = 0;
	libc_ret = read(fd2, buf, sizeof(buf));
	libc_errno = errno;

	ft_read_print("directory fd", ft_ret, ft_errno, NULL, libc_ret, libc_errno, NULL);
	snprintf(note, sizeof(note), "ft_ret=%zd ft_errno=%d libc_ret=%zd libc_errno=%d",
		 ft_ret, ft_errno, libc_ret, libc_errno);
	ft_read_result("directory fd — return value matches libc", ft_ret == libc_ret, note);
	ft_read_result("directory fd — errno matches libc", ft_errno == libc_errno, note);

	close(fd1);
	close(fd2);
}




int main(void)
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
	//ft_strcpy(NULL, "hello");   // writing to NULL
	//ft_strcpy(actual, NULL);    // reading from NULL



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
	
	
	printf("\n============================\n===== ft_read test group.\n============================\n");
	ft_read_test_basic();
	ft_read_test_partial();
	ft_read_test_zero_bytes();
	ft_read_test_eof();
	ft_read_test_invalid_fd();
	ft_read_test_null_buffer();
	ft_read_test_null_buffer_zero_count();
	ft_read_test_write_only_fd();
	ft_read_test_pipe();
	ft_read_test_directory();

	printf("\nft_read test group complete.\n");
	printf("All tests complete.\n");
}