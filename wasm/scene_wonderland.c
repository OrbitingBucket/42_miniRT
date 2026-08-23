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

	body = shiny(mat(0.20, 0.85, 0.62), 0.55, 40);
	i = -1;
	while (++i < 5)
	{
		p = vec3(cap_c.x + seg[i][0], 0, cap_c.z + seg[i][1]);
		p.y = cap_top(cap_c, cap_r, seg[i][0], seg[i][1]) + seg[i][2] * 0.72;
		add_sphere(s, p, seg[i][2], body);
	}
	p = vec3(cap_c.x - 2.35, 0, cap_c.z - 0.55);
	p.y = cap_top(cap_c, cap_r, -2.35, -0.55) + 0.62;
	add_sphere(s, p, 0.85, shiny(mat(0.14, 0.75, 0.58), 0.6, 64));
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
	add_light(s, vec3(-22, 26, -12), 0.75, (t_color){1.00, 0.35, 0.75});
	add_light(s, vec3(11, 19, 31), 0.60, (t_color){0.75, 0.95, 1.00});
	add_light(s, vec3(-1.5, 11, -4), 0.50, (t_color){1.00, 0.75, 0.35});
}
