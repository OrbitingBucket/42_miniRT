#include <stdio.h>
#include <stdlib.h>

unsigned char	*rt_init(void);
void			rt_set_cam(double px, double py, double pz,
					double dx, double dy, double dz);
void			rt_render_band(int y0, int y1, int quality);

int	main(int argc, char **argv)
{
	unsigned char	*buf;
	FILE			*out;
	int				w;
	int				h;

	if (argc < 4)
		return (1);
	w = atoi(argv[2]);
	h = atoi(argv[3]);
	buf = rt_init();
	if (argc >= 10)
		rt_set_cam(atof(argv[4]), atof(argv[5]), atof(argv[6]),
			atof(argv[7]), atof(argv[8]), atof(argv[9]));
	rt_render_band(0, h, 0);
	out = fopen(argv[1], "wb");
	if (!out)
		return (1);
	fwrite(buf, 1, (size_t)w * h * 4, out);
	fclose(out);
	return (0);
}
