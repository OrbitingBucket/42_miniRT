#include "miniRT_bonus.h"
#include <stdlib.h>

static t_object	*obj_new(t_scene *s, t_obj_type type, t_material m)
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

static t_material	mat(double r, double g, double b)
{
	return (default_material((t_color){r, g, b}));
}

static void	add_sphere(t_scene *s, t_vec3 c, double radius, t_material m)
{
	t_object	*o;

	o = obj_new(s, OBJ_SPHERE, m);
	o->sphere.center = c;
	o->sphere.radius = radius;
}

static void	add_plane(t_scene *s, t_vec3 p, t_vec3 n, t_material m)
{
	t_object	*o;

	o = obj_new(s, OBJ_PLANE, m);
	o->plane.point = p;
	o->plane.normal = vec3_norm(n);
}

static void	add_cyl(t_scene *s, t_vec3 base, double r, double h, t_material m)
{
	t_object	*o;

	o = obj_new(s, OBJ_CYLINDER, m);
	o->cylinder.center = vec3_add(base, vec3(0, h / 2.0, 0));
	o->cylinder.axis = vec3(0, 1, 0);
	o->cylinder.radius = r;
	o->cylinder.height = h;
	o->cylinder.top_center = vec3_add(o->cylinder.center,
			vec3_scale(o->cylinder.axis, h / 2.0));
	o->cylinder.bottom_center = vec3_sub(o->cylinder.center,
			vec3_scale(o->cylinder.axis, h / 2.0));
}

static void	add_cyl_axis(t_scene *s, t_vec3 center, t_vec3 axis, double r,
		double h, t_material m)
{
	t_object	*o;

	o = obj_new(s, OBJ_CYLINDER, m);
	o->cylinder.center = center;
	o->cylinder.axis = vec3_norm(axis);
	o->cylinder.radius = r;
	o->cylinder.height = h;
	o->cylinder.top_center = vec3_add(center,
			vec3_scale(o->cylinder.axis, h / 2.0));
	o->cylinder.bottom_center = vec3_sub(center,
			vec3_scale(o->cylinder.axis, h / 2.0));
}

static void	add_cone(t_scene *s, t_vec3 apex, t_vec3 axis, double r,
		double h, t_material m)
{
	t_object	*o;

	o = obj_new(s, OBJ_CONE, m);
	o->cone.apex = apex;
	o->cone.axis = vec3_norm(axis);
	o->cone.radius = r;
	o->cone.height = h;
}

static void	add_light(t_scene *s, t_vec3 pos, double br, t_color c)
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

static t_material	shiny(t_material m, double ks, double shine)
{
	m.ks = ks;
	m.shininess = shine;
	return (m);
}

static t_material	checked(t_material m, double size)
{
	m.checkerboard = 1;
	m.checker_size = size;
	return (m);
}

static void	mushroom(t_scene *s, t_vec3 base, double stem_h, double cap_r,
		t_material cap)
{
	add_cyl(s, base, cap_r * 0.34, stem_h, mat(0.93, 0.88, 0.76));
	add_sphere(s, vec3(base.x, base.y + stem_h + cap_r * 0.28, base.z),
		cap_r, shiny(cap, 0.5, 48));
}

static double	cap_top(t_vec3 cap_c, double cap_r, double dx, double dz)
{
	double	d2;

	d2 = dx * dx + dz * dz;
	if (d2 > cap_r * cap_r * 0.9)
		d2 = cap_r * cap_r * 0.9;
	return (cap_c.y + __builtin_sqrt(cap_r * cap_r - d2));
}

static void	caterpillar(t_scene *s, t_vec3 cap_c, double cap_r)
{
	static const double	seg[5][3] = {{1.75, -1.05, 0.78}, {0.95, -1.55, 0.71},
	{0.05, -1.80, 0.66}, {-0.90, -1.70, 0.60}, {-1.70, -1.25, 0.55}};
	t_material			body;
	t_vec3				p;
	int					i;

	body = shiny(mat(0.18, 0.45, 0.92), 0.55, 40);
	i = -1;
	while (++i < 5)
	{
		p = vec3(cap_c.x + seg[i][0], 0, cap_c.z + seg[i][1]);
		p.y = cap_top(cap_c, cap_r, seg[i][0], seg[i][1]) + seg[i][2] * 0.72;
		add_sphere(s, p, seg[i][2], body);
	}
	p = vec3(cap_c.x - 2.35, 0, cap_c.z - 0.55);
	p.y = cap_top(cap_c, cap_r, -2.35, -0.55) + 0.62;
	add_sphere(s, p, 0.85, shiny(mat(0.12, 0.35, 0.85), 0.6, 64));
	add_cone(s, vec3(p.x - 0.26, p.y + 1.55, p.z - 0.14), vec3(0, -1, 0),
		0.09, 0.82, mat(0.95, 0.35, 0.55));
	add_cone(s, vec3(p.x + 0.32, p.y + 1.50, p.z + 0.10), vec3(0, -1, 0),
		0.09, 0.76, mat(0.95, 0.35, 0.55));
}

static void	hookah(t_scene *s, t_vec3 cap_c, double cap_r)
{
	(void)cap_c;
	(void)cap_r;
	add_cyl(s, vec3(-0.7, 0, -1.4), 0.42, 0.85,
		shiny(mat(0.78, 0.60, 0.20), 0.85, 96));
	add_sphere(s, vec3(-0.7, 1.05, -1.4), 0.40,
		shiny(mat(0.88, 0.70, 0.25), 0.85, 96));
	add_cyl(s, vec3(-0.05, 0, -1.9), 0.07, 2.3,
		shiny(mat(0.85, 0.68, 0.25), 0.85, 96));
}

static void	add_tri(t_scene *s, t_vec3 a, t_vec3 b, t_vec3 c, t_material m)
{
	t_object	*o;

	o = obj_new(s, OBJ_TRIANGLE, m);
	o->triangle.v0 = a;
	o->triangle.v1 = b;
	o->triangle.v2 = c;
	o->triangle.normal = vec3_norm(vec3_cross(vec3_sub(b, a),
				vec3_sub(c, a)));
}

static void	rabbit(t_scene *s, t_vec3 p)
{
	t_material	fur;

	fur = shiny(mat(0.94, 0.92, 0.90), 0.3, 24);
	add_sphere(s, vec3(p.x, p.y + 0.75, p.z), 0.75, fur);
	add_sphere(s, vec3(p.x, p.y + 1.78, p.z - 0.18), 0.48, fur);
	add_cone(s, vec3(p.x - 0.20, p.y + 3.05, p.z - 0.20), vec3(0, -1, 0),
		0.15, 0.95, shiny(mat(0.95, 0.75, 0.80), 0.3, 24));
	add_cone(s, vec3(p.x + 0.22, p.y + 3.00, p.z - 0.16), vec3(0, -1, 0),
		0.15, 0.90, shiny(mat(0.95, 0.75, 0.80), 0.3, 24));
	add_sphere(s, vec3(p.x, p.y + 0.55, p.z + 0.78), 0.24, fur);
}

static void	snail(t_scene *s, t_vec3 p)
{
	add_sphere(s, vec3(p.x, p.y + 0.60, p.z), 0.60,
		checked(shiny(mat(0.66, 0.34, 0.14), 0.5, 48), 0.3));
	add_sphere(s, vec3(p.x + 0.62, p.y + 0.26, p.z + 0.18), 0.26,
		mat(0.85, 0.70, 0.52));
	add_sphere(s, vec3(p.x + 0.88, p.y + 0.50, p.z + 0.26), 0.20,
		mat(0.85, 0.70, 0.52));
}

static void	butterfly(t_scene *s, t_vec3 p, t_color wl, t_color wr)
{
	add_sphere(s, p, 0.13, shiny(mat(0.18, 0.18, 0.28), 0.4, 32));
	add_tri(s, vec3(p.x - 0.05, p.y + 0.05, p.z),
		vec3(p.x - 0.78, p.y + 0.58, p.z + 0.16),
		vec3(p.x - 0.66, p.y - 0.36, p.z + 0.10),
		shiny(default_material(wl), 0.5, 48));
	add_tri(s, vec3(p.x + 0.05, p.y + 0.05, p.z),
		vec3(p.x + 0.78, p.y + 0.60, p.z - 0.14),
		vec3(p.x + 0.66, p.y - 0.34, p.z - 0.10),
		shiny(default_material(wr), 0.5, 48));
}

static void	cheshire(t_scene *s, t_vec3 p)
{
	t_material	coat;

	coat = checked(shiny(mat(0.90, 0.20, 0.55), 0.5, 48), 0.45);
	add_sphere(s, p, 0.95, coat);
	add_sphere(s, vec3(p.x, p.y + 1.15, p.z - 0.25), 0.60, coat);
	add_cone(s, vec3(p.x - 0.32, p.y + 2.15, p.z - 0.25), vec3(0, -1, 0),
		0.17, 0.55, mat(0.90, 0.20, 0.55));
	add_cone(s, vec3(p.x + 0.32, p.y + 2.15, p.z - 0.25), vec3(0, -1, 0),
		0.17, 0.55, mat(0.90, 0.20, 0.55));
	add_cyl_axis(s, vec3(p.x, p.y + 0.92, p.z - 0.80), vec3(1, 0, 0.12),
		0.07, 0.72, shiny(mat(0.98, 0.97, 0.90), 0.7, 96));
	add_sphere(s, vec3(p.x - 0.21, p.y + 1.32, p.z - 0.76), 0.09,
		shiny(mat(0.98, 0.85, 0.20), 0.8, 96));
	add_sphere(s, vec3(p.x + 0.21, p.y + 1.32, p.z - 0.76), 0.09,
		shiny(mat(0.98, 0.85, 0.20), 0.8, 96));
}

static void	queen(t_scene *s, t_vec3 p)
{
	add_cone(s, vec3(p.x, p.y + 2.3, p.z), vec3(0, -1, 0), 1.15, 2.3,
		shiny(mat(0.80, 0.08, 0.18), 0.4, 32));
	add_sphere(s, vec3(p.x, p.y + 2.6, p.z), 0.50,
		shiny(mat(0.62, 0.06, 0.20), 0.4, 32));
	add_sphere(s, vec3(p.x, p.y + 3.35, p.z), 0.34, mat(0.95, 0.85, 0.75));
	add_cone(s, vec3(p.x, p.y + 4.25, p.z), vec3(0, -1, 0), 0.32, 0.55,
		shiny(mat(0.95, 0.80, 0.20), 0.9, 128));
}

static void	card(t_scene *s, double x, double z, double lean, int mark)
{
	t_material	face;

	face = shiny(mat(0.94, 0.94, 0.90), 0.2, 16);
	add_tri(s, vec3(x - 0.55, 0, z), vec3(x + 0.55, 0, z + 0.05),
		vec3(x + 0.55, 1.7, z + lean), face);
	add_tri(s, vec3(x - 0.55, 0, z), vec3(x + 0.55, 1.7, z + lean),
		vec3(x - 0.55, 1.7, z + lean - 0.05), face);
	if (mark)
		add_sphere(s, vec3(x, 0.9, z + lean * 0.5 - 0.10), 0.15,
			shiny(mat(0.85, 0.08, 0.15), 0.5, 48));
}

static void	dodo(t_scene *s, t_vec3 p)
{
	add_sphere(s, vec3(p.x, p.y + 0.85, p.z), 0.70, mat(0.62, 0.55, 0.45));
	add_sphere(s, vec3(p.x + 0.55, p.y + 1.75, p.z), 0.35,
		mat(0.68, 0.60, 0.50));
	add_cone(s, vec3(p.x + 1.50, p.y + 1.68, p.z), vec3(-1, 0.05, 0),
		0.16, 0.62, shiny(mat(0.90, 0.60, 0.20), 0.5, 48));
	add_cyl(s, vec3(p.x - 0.22, p.y, p.z + 0.12), 0.06, 0.45,
		mat(0.90, 0.60, 0.20));
	add_cyl(s, vec3(p.x + 0.20, p.y, p.z - 0.10), 0.06, 0.45,
		mat(0.90, 0.60, 0.20));
}

static void	birds(t_scene *s)
{
	add_tri(s, vec3(-11, 14, 9), vec3(-10.2, 14.5, 9.2),
		vec3(-10.6, 13.9, 8.6), mat(0.16, 0.13, 0.28));
	add_tri(s, vec3(-9.2, 15.2, 11), vec3(-8.5, 15.6, 11.2),
		vec3(-8.9, 15.0, 10.5), mat(0.16, 0.13, 0.28));
	add_tri(s, vec3(-12.5, 13.2, 12), vec3(-11.8, 13.7, 12.2),
		vec3(-12.2, 13.1, 11.5), mat(0.16, 0.13, 0.28));
}

static void	creatures(t_scene *s)
{
	rabbit(s, vec3(3.2, 0, -6.2));
	snail(s, vec3(-3.6, 0, -7.5));
	butterfly(s, vec3(-1.2, 4.2, -1.5),
		(t_color){0.95, 0.30, 0.60}, (t_color){0.98, 0.60, 0.15});
	butterfly(s, vec3(3.6, 2.8, -4.6),
		(t_color){0.62, 0.40, 0.95}, (t_color){0.35, 0.90, 0.55});
	butterfly(s, vec3(0.5, 6.5, -8.0),
		(t_color){0.95, 0.75, 0.20}, (t_color){0.90, 0.25, 0.25});
	butterfly(s, vec3(-4.5, 7.2, 1.0),
		(t_color){0.40, 0.85, 0.95}, (t_color){0.95, 0.45, 0.75});
	cheshire(s, vec3(-8.5, 5.0, 3.5));
	queen(s, vec3(7.5, 0, -6.5));
	card(s, 5.6, -8.6, 0.15, 1);
	card(s, 4.7, -9.8, -0.10, 0);
	card(s, 6.4, -10.6, 0.20, 1);
	dodo(s, vec3(-6.8, 0, -12));
	birds(s);
}

static void	surroundings(t_scene *s)
{
	mushroom(s, vec3(-5.2, 0, -16), 1.8, 1.10, mat(0.72, 0.90, 0.20));
	mushroom(s, vec3(6.5, 0, -13.5), 2.6, 1.40,
		checked(mat(0.20, 0.85, 0.75), 0.5));
	mushroom(s, vec3(-10.5, 0, -9), 4.0, 1.80, mat(0.92, 0.45, 0.72));
	add_cone(s, vec3(-13, 11, -19), vec3(0, -1, 0), 3.6, 11,
		mat(0.12, 0.08, 0.26));
	add_sphere(s, vec3(4.2, 5.2, -11.5), 0.62,
		shiny(mat(0.98, 0.72, 0.50), 0.8, 128));
}

static void	fairy_ring(t_scene *s)
{
	mushroom(s, vec3(3.4, 0, -5.2), 0.55, 0.42, mat(0.95, 0.72, 0.12));
	mushroom(s, vec3(1.4, 0, -3.6), 0.70, 0.50, mat(0.68, 0.45, 0.95));
}

static void	scatter(t_scene *s)
{
	add_sphere(s, vec3(-1.2, 0.30, -8.9), 0.60,
		shiny(mat(0.92, 0.70, 0.20), 0.5, 64));
	add_sphere(s, vec3(-2.1, 0.32, -5.6), 0.60,
		shiny(mat(0.90, 0.42, 0.62), 0.5, 64));
	add_sphere(s, vec3(-5.5, 5.2, -4.5), 0.72,
		checked(shiny(mat(0.72, 0.50, 0.95), 0.8, 128), 0.4));
	add_sphere(s, vec3(0.6, 8.2, 6.5), 0.92,
		shiny(mat(0.45, 0.85, 0.98), 0.8, 128));
	add_cone(s, vec3(-7.3, 2.7, -1.4), vec3(0, -1, 0), 0.85, 2.7,
		shiny(mat(0.12, 0.90, 0.50), 0.9, 128));
	add_cone(s, vec3(-6.4, 1.8, -0.4), vec3(0, -1, 0), 0.55, 1.8,
		shiny(mat(0.95, 0.78, 0.18), 0.9, 128));
	add_cone(s, vec3(-17, 13.5, 30), vec3(0, -1, 0), 4.4, 13.5,
		mat(0.10, 0.07, 0.22));
	add_sphere(s, vec3(12, 20, 34), 2.8, shiny(mat(0.98, 0.95, 0.82), 0, 1));
}

void	build_wonderland(t_scene *s)
{
	t_vec3	big_cap;

	s->objects = NULL;
	s->lights = NULL;
	s->ambient = (t_ambient){0.22, (t_color){0.60, 0.50, 1.00}};
	s->camera = (t_camera){vec3(0, 2.2, -11.5), vec3(0, 0.12, 1), 72};
	s->has_ambient = 1;
	s->has_camera = 1;
	s->has_light = 1;
	add_sphere(s, vec3(0, 0, 0), 260, shiny(mat(0.10, 0.06, 0.22), 0, 1));
	add_plane(s, vec3(0, 0, 0), vec3(0, 1, 0),
		checked(shiny(mat(0.48, 0.28, 0.62), 0.06, 16), 4.2));
	big_cap = vec3(-3, 4.55, 2);
	add_cyl(s, vec3(-3, 0, 2), 1.1, 3.7, mat(0.93, 0.88, 0.76));
	add_sphere(s, big_cap, 3.3, checked(shiny(mat(0.86, 0.12, 0.28),
				0.55, 48), 1.1));
	caterpillar(s, big_cap, 3.3);
	hookah(s, big_cap, 3.3);
	mushroom(s, vec3(4.2, 0, 4.5), 2.2, 1.30, mat(0.95, 0.45, 0.10));
	mushroom(s, vec3(5.7, 0, 2.4), 1.6, 1.00,
		checked(mat(0.95, 0.35, 0.65), 0.5));
	mushroom(s, vec3(8.5, 0, 13), 10.5, 2.00,
		checked(mat(0.20, 0.80, 0.90), 0.8));
	fairy_ring(s);
	scatter(s);
	creatures(s);
	surroundings(s);
	add_light(s, vec3(-22, 26, -12), 0.75, (t_color){1.00, 0.35, 0.75});
	add_light(s, vec3(11, 19, 31), 0.60, (t_color){0.75, 0.95, 1.00});
	add_light(s, vec3(-1.5, 11, -4), 0.50, (t_color){1.00, 0.75, 0.35});
}
