#include <unistd.h>
#include <stdio.h>

int main()
{
	char buf[2];
	int i;

	buf[1] = '\n';

	for (i = 0; i < 5; ++i)
	{
		sleep(1);
		buf[0] = '0' + i;
		write(STDOUT_FILENO, buf, 2);
	}

	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	sleep(5);
	return 0;
}
