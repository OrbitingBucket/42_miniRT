/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miniRT.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 16:05:29 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 12:51:33 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include "libft.h"
# include <fcntl.h>
# include <math.h>
# include <mlx.h>
# include <unistd.h>

# define EPSILON 1e-6
# define WIDTH 800
# define HEIGHT 600
# define MAX_DEPTH 8
# define SHADOW_BIAS 1e-4

# ifdef __linux__
#  define KEY_ESC 65307
#  define KEY_W 119
#  define KEY_A 97
#  define KEY_S 115
#  define KEY_D 100
#  define KEY_UP 65362
#  define KEY_DOWN 65364
#  define KEY_LEFT 65361
#  define KEY_RIGHT 65363
#  define KEY_SPACE 32
# endif

typedef struct s_vec3
{
	double			x;
	double			y;
	double			z;
}					t_vec3;

typedef struct s_ray
{
	t_vec3			origin;
	t_vec3			dir;
}					t_ray;

typedef struct s_color
{
	double			r;
	double			g;
	double			b;
}					t_color;

typedef struct s_ambient
{
	double			ratio;
	t_color			color;
}					t_ambient;

typedef struct s_camera
{
	t_vec3			pos;
	t_vec3			dir;
	double			fov;
}					t_camera;

typedef struct s_light
{
	t_vec3			pos;
	double			brightness;
	t_color			color;
	double			kc;
	double			kl;
	double			kq;
	struct s_light	*next;
}					t_light;

typedef struct s_material
{
	t_color			color;
	double			ka;
	double			kd;
	double			ks;
	double			shininess;
	double			reflectivity;
	double			transparency;
	double			ior;
	double			roughness;
	int				checkerboard;
	double			checker_size;
}					t_material;

typedef struct s_sphere
{
	t_vec3			center;
	double			radius;
}					t_sphere;

typedef struct s_plane
{
	t_vec3			point;
	t_vec3			normal;
}					t_plane;

typedef struct s_cylinder
{
	t_vec3			center;
	t_vec3			axis;
	double			radius;
	double			height;
	t_vec3			top_center;
	t_vec3			bottom_center;

}					t_cylinder;

typedef struct s_cone
{
	t_vec3			apex;
	t_vec3			axis;
	double			radius;
	double			height;
}					t_cone;

typedef struct s_triangle
{
	t_vec3			v0;
	t_vec3			v1;
	t_vec3			v2;
	t_vec3			normal;
}					t_triangle;

typedef enum e_obj_type
{
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER,
	OBJ_CONE,
	OBJ_TRIANGLE,
}					t_obj_type;

typedef struct s_object
{
	t_obj_type		type;
	union
	{
		t_sphere	sphere;
		t_plane		plane;
		t_cylinder	cylinder;
		t_cone		cone;
		t_triangle	triangle;
	};
	t_material		mat;
	struct s_object	*next;
}					t_object;

typedef struct s_quad
{
	double			a;
	double			b;
	double			c;
	double			disc;
	double			sqrt_disc;
	t_vec3			d;
	t_vec3			f;
	double			m;
	double			n;
	double			root;
}					t_quad;

typedef struct s_scene
{
	t_ambient		ambient;
	t_camera		camera;
	t_light			*lights;
	t_object		*objects;
	int				has_ambient;
	int				has_camera;
	int				has_light;
}					t_scene;

typedef struct s_mlx
{
	void			*ptr;
	void			*win;
	void			*img;
	char			*addr;
	int				bpp;
	int				line_len;
	int				endian;
}					t_mlx;

typedef struct s_app
{
	t_mlx			mlx;
	t_scene			scene;
	int				is_locked;
}					t_app;

typedef struct s_camera_basis
{
	t_vec3			origin;
	t_vec3			horizontal;
	t_vec3			vertical;
	t_vec3			lower_left;
}					t_camera_basis;

typedef struct s_hit
{
	double			t;
	t_vec3			point;
	t_vec3			normal;
	t_material		*mat;
	int				front_face;
}					t_hit;

t_vec3				vec3(double x, double y, double z);
t_vec3				vec3_add(t_vec3 a, t_vec3 b);
t_vec3				vec3_sub(t_vec3 a, t_vec3 b);
t_vec3				vec3_scale(t_vec3 v, double t);
t_vec3				vec3_negate(t_vec3 v);
double				vec3_dot(t_vec3 a, t_vec3 b);
t_vec3				vec3_cross(t_vec3 a, t_vec3 b);
double				vec3_len(t_vec3 v);
t_vec3				vec3_norm(t_vec3 v);
t_vec3				vec3_reflect(t_vec3 v, t_vec3 n);
int					vec3_near_zero(t_vec3 v);
t_vec3				ray_at(t_ray ray, double t);
int					quad_solve(t_quad *q);
double				quad_root1(t_quad *q);
double				quad_root2(t_quad *q);
int					quad_nearest(t_quad *q, double *t);

void				parse_scene(const char *file, t_scene *scene);
void				parse_ambient(char **tokens, t_scene *scene);
void				parse_camera(char **tokens, t_scene *scene);
void				parse_light(char **tokens, t_scene *scene);
void				parse_material(char **tokens, int color_idx,
						t_material *mat);
t_material			default_material(t_color color);
t_object			*new_object(t_obj_type type);
void				append_object(t_scene *scene, t_object *obj);
void				parse_sphere(char **tokens, t_scene *scene);
void				parse_plane(char **tokens, t_scene *scene);
void				parse_cylinder(char **tokens, t_scene *scene);
void				dispatch(char **tokens, t_scene *scene);
void				check_count(char **tokens, int n, char *msg);
int					parse_sign(const char **str);
double				parse_integer(const char **str);
double				parse_fraction(const char **str);
double				ft_strtod(const char *str, const char **endptr);
int					valid_num(const char *s);
double				next_component(const char **s, int last);
double				parse_double(const char *s);
double				parse_ratio(const char *s);
t_color				parse_color(const char *s);
t_vec3				parse_vec3(const char *s);
t_vec3				parse_normal(const char *s);
char				**split_line(const char *line);
void				free_tokens(char **tokens);
int					token_count(char **tokens);
void				error_exit(const char *msg);
void				free_scene(t_scene *scene);

void				mlx_setup(t_app *app);
void				mlx_put_pixel(t_mlx *mlx, int x, int y, t_color color);
int					mouse_hook(int button, int x, int y, void *param);
int					key_handler(int keycode, void *param);
int					close_handler(t_app *app);
int					expose_handler(t_app *app);

void				move_camera(int key, t_camera *cam, double speed);
t_camera_basis		build_camera_basis(t_camera *cam);
t_ray				get_ray(t_camera_basis *basis, double u, double v);
t_color				ray_color(t_ray ray, t_scene *scene, int depth);
void				render(t_app *app);

int					intersect_scene(t_ray ray, t_scene *scene, t_hit *hit);
int					intersect_sphere(t_ray ray, t_object *obj, t_hit *hit);
int					intersect_plane(t_ray ray, t_object *obj, t_hit *hit);
int					cylinder_quadratic(t_ray ray, t_object *obj, t_quad *q);
t_vec3				body_normal(t_object *obj, t_vec3 point);
int					check_body_hit(t_ray ray, t_object *obj, t_hit *hit,
						double t);
void				set_cap_hit(t_ray ray, t_object *obj, t_hit *hit,
						t_vec3 normal);
int					intersect_cylinder(t_ray ray, t_object *obj, t_hit *hit);
int					hit_object(t_ray ray, t_object *obj, t_hit *tmp);

t_color				shade(t_hit *hit, t_scene *scene, t_ray ray);
t_color				ambient_light(t_hit *hit, t_ambient *ambient);
t_color				diffuse_light(t_hit *hit, t_light *light);
int					in_shadow(t_scene *scene, t_hit *hit, t_light *light);
double				attenuation(t_light *light, t_vec3 point);
t_color				color_scale(t_color c, double t);
t_color				color_add(t_color a, t_color b);
t_color				color_multiply(t_color a, t_color b);
t_color				color_clamp(t_color c);
t_color				color_lerp(t_color a, t_color b, double t);

#endif
