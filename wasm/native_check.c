#include <stdio.h>
#include <stdlib.h>

unsigned char	*rt_init(void);
int				rt_tick(void);

int	main(int argc, char **argv)
{
	unsigned char	*buf;
	FILE			*out;
	int				frames;

	if (argc != 2)
		return (1);
	buf = rt_init();
	frames = 0;
	while (frames < 2)
		frames += rt_tick();
	out = fopen(argv[1], "wb");
	if (!out)
		return (1);
	fwrite(buf, 1, (size_t)800 * 600 * 4, out);
	fclose(out);
	printf("native full frame written\n");
	return (0);
}
