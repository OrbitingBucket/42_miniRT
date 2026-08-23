#include "miniRT_bonus.h"
#include <stdlib.h>

void	accel_open(t_scene *s);
void	accel_close(t_scene *s);

static t_object	*cobj(t_scene *s, t_obj_type type, t_material m)
{
	t_object	*o;

	o = malloc(sizeof(t_object));
	if (!o)
		error_exit("malloc failed");
	o->type = type;
	o->mat = m;
	o->next = s->objects;
	s->objects = o;
	return (o);
}

static t_material	cmat(double r, double g, double b, double ks)
{
	t_material	m;

	m = default_material((t_color){r, g, b});
	m.ks = ks;
	m.shininess = 80.0;
	return (m);
}

static void	csphere(t_scene *s, t_vec3 c, double radius, t_material m)
{
	t_object	*o;

	o = cobj(s, OBJ_SPHERE, m);
	o->sphere.center = c;
	o->sphere.radius = radius;
}

static void	ccyl(t_scene *s, t_vec3 center, t_vec3 axis, double r, double h,
		t_material m)
{
	t_object	*o;

	o = cobj(s, OBJ_CYLINDER, m);
	o->cylinder.center = center;
	o->cylinder.axis = vec3_norm(axis);
	o->cylinder.radius = r;
	o->cylinder.height = h;
	o->cylinder.top_center = vec3_add(center,
			vec3_scale(o->cylinder.axis, h / 2.0));
	o->cylinder.bottom_center = vec3_sub(center,
			vec3_scale(o->cylinder.axis, h / 2.0));
}

static void	ccone(t_scene *s, t_vec3 apex, t_vec3 axis, double r, double h,
		t_material m)
{
	t_object	*o;

	o = cobj(s, OBJ_CONE, m);
	o->cone.apex = apex;
	o->cone.axis = vec3_norm(axis);
	o->cone.radius = r;
	o->cone.height = h;
}

static void	cplane(t_scene *s, t_vec3 p, t_vec3 n, t_material m)
{
	t_object	*o;

	o = cobj(s, OBJ_PLANE, m);
	o->plane.point = p;
	o->plane.normal = vec3_norm(n);
}

static void	ctri(t_scene *s, t_vec3 a, t_vec3 b, t_vec3 c, t_material m)
{
	t_object	*o;

	o = cobj(s, OBJ_TRIANGLE, m);
	o->triangle.v0 = a;
	o->triangle.v1 = b;
	o->triangle.v2 = c;
	o->triangle.normal = vec3_norm(vec3_cross(vec3_sub(b, a),
				vec3_sub(c, a)));
}

static void	clathe(t_scene *s, t_vec3 base, const double (*prof)[2],
		int np, int nseg, t_material m)
{
	int		i;
	int		k;
	double	a0;
	double	a1;
	t_vec3	q[4];

	i = -1;
	while (++i < np - 1)
	{
		k = -1;
		while (++k < nseg)
		{
			a0 = 6.28318530718 * k / nseg;
			a1 = 6.28318530718 * (k + 1) / nseg;
			q[0] = vec3(base.x + prof[i][0] * __builtin_cos(a0),
					base.y + prof[i][1], base.z + prof[i][0] * __builtin_sin(a0));
			q[1] = vec3(base.x + prof[i][0] * __builtin_cos(a1),
					base.y + prof[i][1], base.z + prof[i][0] * __builtin_sin(a1));
			q[2] = vec3(base.x + prof[i + 1][0] * __builtin_cos(a0),
					base.y + prof[i + 1][1],
					base.z + prof[i + 1][0] * __builtin_sin(a0));
			q[3] = vec3(base.x + prof[i + 1][0] * __builtin_cos(a1),
					base.y + prof[i + 1][1],
					base.z + prof[i + 1][0] * __builtin_sin(a1));
			if (prof[i][0] > 0.001)
				ctri(s, q[0], q[1], q[3], m);
			if (prof[i + 1][0] > 0.001)
				ctri(s, q[0], q[3], q[2], m);
		}
	}
}

static void	pawn(t_scene *s, double x, double z, t_material m)
{
	static const double	pp[8][2] = {{0.30, 0.0}, {0.30, 0.05}, {0.19, 0.12},
	{0.11, 0.38}, {0.16, 0.50}, {0.09, 0.55}, {0.155, 0.66}, {0.02, 0.80}};

	accel_open(s);
	clathe(s, vec3(x, 0.02, z), pp, 8, 8, m);
	accel_close(s);
}

static void	rook(t_scene *s, double x, double z, t_material m)
{
	static const double	rp[8][2] = {{0.33, 0.0}, {0.33, 0.06}, {0.22, 0.14},
	{0.165, 0.32}, {0.155, 0.64}, {0.235, 0.72}, {0.235, 0.92}, {0.02, 0.92}};
	int					k;

	accel_open(s);
	clathe(s, vec3(x, 0.02, z), rp, 8, 10, m);
	k = -1;
	while (++k < 4)
		ccyl(s, vec3(x + 0.17 * __builtin_cos(k * 1.5708 + 0.785),
				1.00, z + 0.17 * __builtin_sin(k * 1.5708 + 0.785)),
			vec3(0, 1, 0), 0.055, 0.15, m);
	accel_close(s);
}

static void	knight(t_scene *s, double x, double z, t_material m, double fx)
{
	static const double	kp[5][2] = {{0.34, 0.0}, {0.34, 0.06}, {0.23, 0.14},
	{0.19, 0.26}, {0.165, 0.34}};

	accel_open(s);
	clathe(s, vec3(x, 0.02, z), kp, 5, 10, m);
	ccyl(s, vec3(x + fx * 0.05, 0.66, z), vec3(fx * 0.42, 1, 0),
		0.15, 0.62, m);
	csphere(s, vec3(x + fx * 0.17, 1.00, z), 0.165, m);
	ccone(s, vec3(x + fx * 0.47, 0.87, z), vec3(-fx, 0.38, 0),
		0.115, 0.40, m);
	ccone(s, vec3(x + fx * 0.07, 1.24, z - 0.07), vec3(0, -1, 0),
		0.045, 0.13, m);
	ccone(s, vec3(x + fx * 0.07, 1.24, z + 0.07), vec3(0, -1, 0),
		0.045, 0.13, m);
	accel_close(s);
}

static void	bishop(t_scene *s, double x, double z, t_material m)
{
	static const double	bp[10][2] = {{0.33, 0.0}, {0.33, 0.06}, {0.22, 0.14},
	{0.13, 0.36}, {0.10, 0.62}, {0.175, 0.74}, {0.085, 0.80}, {0.15, 0.92},
	{0.11, 1.08}, {0.02, 1.18}};

	accel_open(s);
	clathe(s, vec3(x, 0.02, z), bp, 10, 10, m);
	csphere(s, vec3(x, 1.24, z), 0.05, m);
	accel_close(s);
}

static void	queen(t_scene *s, double x, double z, t_material m)
{
	static const double	qp[11][2] = {{0.36, 0.0}, {0.36, 0.06}, {0.25, 0.15},
	{0.15, 0.40}, {0.115, 0.74}, {0.10, 1.00}, {0.21, 1.14}, {0.105, 1.20},
	{0.185, 1.32}, {0.08, 1.40}, {0.02, 1.46}};
	int					k;

	accel_open(s);
	clathe(s, vec3(x, 0.02, z), qp, 11, 10, m);
	k = -1;
	while (++k < 5)
		csphere(s, vec3(x + 0.155 * __builtin_cos(k * 1.2566),
				1.38, z + 0.155 * __builtin_sin(k * 1.2566)), 0.045, m);
	csphere(s, vec3(x, 1.52, z), 0.06, m);
	accel_close(s);
}

static void	king(t_scene *s, double x, double z, t_material m)
{
	static const double	kp[11][2] = {{0.38, 0.0}, {0.38, 0.06}, {0.26, 0.15},
	{0.16, 0.42}, {0.125, 0.82}, {0.105, 1.08}, {0.24, 1.24}, {0.11, 1.30},
	{0.20, 1.42}, {0.09, 1.52}, {0.02, 1.58}};

	accel_open(s);
	clathe(s, vec3(x, 0.02, z), kp, 11, 10, m);
	ccyl(s, vec3(x, 1.70, z), vec3(0, 1, 0), 0.035, 0.26, m);
	ccyl(s, vec3(x, 1.74, z), vec3(1, 0, 0), 0.035, 0.18, m);
	accel_close(s);
}

static void	army(t_scene *s, double back, double front, t_material m,
		double fx)
{
	int	f;

	f = -1;
	while (++f < 8)
		pawn(s, front, f - 3.5, m);
	rook(s, back, -3.5, m);
	rook(s, back, 3.5, m);
	knight(s, back, -2.5, m, fx);
	knight(s, back, 2.5, m, fx);
	bishop(s, back, -1.5, m);
	bishop(s, back, 1.5, m);
	queen(s, back, -0.5, m);
	king(s, back, 0.5, m);
}

static void	board(t_scene *s)
{
	t_material	top;
	t_material	rim;

	top = cmat(0.16, 0.10, 0.08, 0.15);
	top.checkerboard = 1;
	top.checker_size = 1.0;
	rim = cmat(0.30, 0.17, 0.10, 0.3);
	ctri(s, vec3(-4, 0.02, -4), vec3(4, 0.02, -4), vec3(4, 0.02, 4), top);
	ctri(s, vec3(-4, 0.02, -4), vec3(4, 0.02, 4), vec3(-4, 0.02, 4), top);
	ctri(s, vec3(-4.4, 0.001, -4.4), vec3(4.4, 0.001, -4.4),
		vec3(4.4, 0.001, 4.4), rim);
	ctri(s, vec3(-4.4, 0.001, -4.4), vec3(4.4, 0.001, 4.4),
		vec3(-4.4, 0.001, 4.4), rim);
	ccyl(s, vec3(0, -0.22, 0), vec3(0, 1, 0), 5.1, 0.44,
		cmat(0.20, 0.11, 0.07, 0.2));
}

static void	add_clight(t_scene *s, t_vec3 pos, double br, t_color c)
{
	t_light	*l;

	l = malloc(sizeof(t_light));
	if (!l)
		error_exit("malloc failed");
	l->pos = pos;
	l->brightness = br;
	l->color = c;
	l->kc = 1.0;
	l->kl = 0.0;
	l->kq = 0.0;
	l->next = s->lights;
	s->lights = l;
}

void	build_chess(t_scene *s)
{
	t_material	ivory;
	t_material	onyx;

	s->objects = NULL;
	s->lights = NULL;
	s->ambient = (t_ambient){0.22, (t_color){0.72, 0.76, 1.00}};
	s->camera = (t_camera){vec3(0, 4.6, -9.8),
		vec3_norm(vec3(0, -0.36, 1)), 58};
	s->has_ambient = 1;
	s->has_camera = 1;
	s->has_light = 1;
	csphere(s, vec3(0, 0, 0), 200, cmat(0.045, 0.05, 0.10, 0));
	cplane(s, vec3(0, -0.44, 0), vec3(0, 1, 0), cmat(0.15, 0.14, 0.18, 0.1));
	board(s);
	ivory = cmat(0.93, 0.89, 0.80, 0.5);
	onyx = cmat(0.12, 0.11, 0.12, 0.85);
	onyx.shininess = 140.0;
	army(s, -3.5, -2.5, ivory, 1.0);
	army(s, 3.5, 2.5, onyx, -1.0);
	add_clight(s, vec3(-12, 14, -6), 0.70, (t_color){0.95, 0.90, 0.80});
	add_clight(s, vec3(14, 11, -5), 0.45, (t_color){0.50, 0.65, 1.00});
	add_clight(s, vec3(0, 16, 10), 0.30, (t_color){1.00, 1.00, 1.00});
}
