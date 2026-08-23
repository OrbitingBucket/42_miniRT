#include "miniRT_bonus.h"

#define MAX_OBJ 1200
#define MAX_GRP 40

typedef struct s_bound
{
	t_object	*obj;
	t_vec3		c;
	double		r2;
	int			always;
}	t_bound;

typedef struct s_grp
{
	t_object	*head;
	t_object	*end;
	t_vec3		c;
	double		r2;
	int			first;
	int			count;
}	t_grp;

static t_bound	g_b[MAX_OBJ];
static int		g_nb;
static t_grp	g_g[MAX_GRP];
static int		g_ng;
static t_bound	g_loose[MAX_OBJ];
static int		g_nl;
static t_object	*g_open_head;

void	accel_open(t_scene *s)
{
	g_open_head = s->objects;
}

void	accel_close(t_scene *s)
{
	if (g_ng >= MAX_GRP)
		return ;
	g_g[g_ng].head = s->objects;
	g_g[g_ng].end = g_open_head;
	g_ng++;
}

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
	else if (o->type == OBJ_CYLINDER)
	{
		half = o->cylinder.height / 2.0;
		b->c = o->cylinder.center;
		b->r2 = half * half + o->cylinder.radius * o->cylinder.radius;
	}
	else if (o->type == OBJ_CONE)
	{
		half = o->cone.height / 2.0;
		b->c = vec3_add(o->cone.apex, vec3_scale(o->cone.axis, half));
		b->r2 = half * half + o->cone.radius * o->cone.radius;
	}
	else if (o->type == OBJ_TRIANGLE)
	{
		b->c = vec3_scale(vec3_add(vec3_add(o->triangle.v0, o->triangle.v1),
					o->triangle.v2), 1.0 / 3.0);
		half = tri_radius(&o->triangle, b->c);
		b->r2 = half * half * 1.02;
	}
	else
		b->always = 1;
}

static void	group_bound(t_grp *g)
{
	t_vec3	sum;
	double	r;
	double	d;
	int		i;

	sum = vec3(0, 0, 0);
	i = g->first - 1;
	while (++i < g->first + g->count)
		sum = vec3_add(sum, g_b[i].c);
	g->c = vec3_scale(sum, 1.0 / g->count);
	r = 0;
	i = g->first - 1;
	while (++i < g->first + g->count)
	{
		d = vec3_len(vec3_sub(g_b[i].c, g->c)) + __builtin_sqrt(g_b[i].r2);
		if (d > r)
			r = d;
	}
	g->r2 = r * r * 1.02;
}

static int	in_group(t_object *o, int *gi)
{
	int	k;

	k = -1;
	while (++k < g_ng)
	{
		t_object	*p = g_g[k].head;
		while (p && p != g_g[k].end)
		{
			if (p == o)
			{
				*gi = k;
				return (1);
			}
			p = p->next;
		}
	}
	return (0);
}

void	build_accel(t_scene *s)
{
	t_object	*o;
	int			gi;
	int			k;

	g_nb = 0;
	g_nl = 0;
	k = -1;
	while (++k < g_ng)
	{
		g_g[k].first = g_nb;
		g_g[k].count = 0;
		o = g_g[k].head;
		while (o && o != g_g[k].end && g_nb < MAX_OBJ)
		{
			bound_one(o, &g_b[g_nb++]);
			g_g[k].count++;
			o = o->next;
		}
		group_bound(&g_g[k]);
	}
	o = s->objects;
	while (o && g_nl < MAX_OBJ)
	{
		if (!in_group(o, &gi))
			bound_one(o, &g_loose[g_nl++]);
		o = o->next;
	}
}

static int	miss(t_bound *b, t_ray *ray)
{
	t_vec3	oc;
	double	oc2;
	double	pb;

	if (b->always)
		return (0);
	oc = vec3_sub(b->c, ray->origin);
	oc2 = vec3_dot(oc, oc);
	if (oc2 <= b->r2)
		return (0);
	pb = vec3_dot(oc, ray->dir);
	return (pb < 0 || oc2 - pb * pb > b->r2);
}

static void	test_one(t_bound *b, t_ray *ray, t_hit *hit, double *closest)
{
	t_hit	tmp;

	if (miss(b, ray))
		return ;
	tmp.t = *closest;
	if (hit_object(*ray, b->obj, &tmp) && tmp.t < *closest)
	{
		*closest = tmp.t;
		*hit = tmp;
		hit->t = tmp.t;
	}
}

int	intersect_scene(t_ray ray, t_scene *scene, t_hit *hit)
{
	double	closest;
	int		k;
	int		i;
	t_grp	gb;

	(void)scene;
	closest = 1e15;
	k = -1;
	while (++k < g_ng)
	{
		gb.c = g_g[k].c;
		gb.r2 = g_g[k].r2;
		if (miss((t_bound *)&(t_bound){NULL, gb.c, gb.r2, 0}, &ray))
			continue ;
		i = g_g[k].first - 1;
		while (++i < g_g[k].first + g_g[k].count)
			test_one(&g_b[i], &ray, hit, &closest);
	}
	i = -1;
	while (++i < g_nl)
		test_one(&g_loose[i], &ray, hit, &closest);
	return (closest < 1e14);
}
