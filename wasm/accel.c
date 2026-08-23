#include "miniRT_bonus.h"

typedef struct s_bound
{
	t_object	*obj;
	t_vec3		c;
	double		r2;
	int			always;
}	t_bound;

static t_bound	g_b[256];
static int		g_nb;

static double	tri_radius(t_triangle *t, t_vec3 c)
{
	double	r;
	double	d;

	r = vec3_len(vec3_sub(t->v0, c));
	d = vec3_len(vec3_sub(t->v1, c));
	if (d > r)
		r = d;
	d = vec3_len(vec3_sub(t->v2, c));
	if (d > r)
		r = d;
	return (r);
}

static void	bound_one(t_object *o, t_bound *b)
{
	double	half;

	b->obj = o;
	b->always = 0;
	if (o->type == OBJ_SPHERE)
	{
		b->c = o->sphere.center;
		b->r2 = o->sphere.radius * o->sphere.radius;
	}
	else if (o->type == OBJ_CYLINDER || o->type == OBJ_CONE)
	{
		half = o->cylinder.height / 2.0;
		if (o->type == OBJ_CONE)
		{
			b->c = vec3_add(o->cone.apex, vec3_scale(o->cone.axis, half));
			b->r2 = half * half + o->cone.radius * o->cone.radius;
		}
		else
		{
			b->c = o->cylinder.center;
			b->r2 = half * half + o->cylinder.radius * o->cylinder.radius;
		}
	}
	else if (o->type == OBJ_TRIANGLE)
	{
		b->c = vec3_scale(vec3_add(vec3_add(o->triangle.v0, o->triangle.v1),
					o->triangle.v2), 1.0 / 3.0);
		b->r2 = tri_radius(&o->triangle, b->c);
		b->r2 = b->r2 * b->r2 * 1.02;
	}
	else
		b->always = 1;
}

void	build_accel(t_scene *s)
{
	t_object	*o;

	g_nb = 0;
	o = s->objects;
	while (o && g_nb < 256)
	{
		bound_one(o, &g_b[g_nb]);
		g_nb++;
		o = o->next;
	}
}

int	intersect_scene(t_ray ray, t_scene *scene, t_hit *hit)
{
	t_hit	tmp;
	int		hit_anything;
	double	closest;
	int		i;

	(void)scene;
	hit_anything = 0;
	closest = 1e15;
	i = -1;
	while (++i < g_nb)
	{
		if (!g_b[i].always)
		{
			t_vec3	oc = vec3_sub(g_b[i].c, ray.origin);
			double	oc2 = vec3_dot(oc, oc);
			double	pb = vec3_dot(oc, ray.dir);
			if (oc2 > g_b[i].r2 && (pb < 0 || oc2 - pb * pb > g_b[i].r2))
				continue ;
		}
		tmp.t = closest;
		if (hit_object(ray, g_b[i].obj, &tmp) && tmp.t < closest)
		{
			closest = tmp.t;
			*hit = tmp;
			hit_anything = 1;
		}
	}
	return (hit_anything);
}
