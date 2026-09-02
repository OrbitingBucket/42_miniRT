/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_objects.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 16:51:31 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/10 17:18:10 by jinliang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

t_object	*new_object(t_obj_type type)
{
	t_object	*obj;

	obj = malloc(sizeof(t_object));
	if (!obj)
		error_exit("malloc failed");
	obj->type = type;
	obj->next = NULL;
	return (obj);
}

void	append_object(t_scene *scene, t_object *obj)
{
	t_object	*cursor;

	if (!scene->objects)
	{
		scene->objects = obj;
		return ;
	}
	cursor = scene->objects;
	while (cursor->next)
		cursor = cursor->next;
	cursor->next = obj;
}

void	parse_sphere(char **tokens, t_scene *scene)
{
	t_object	*obj;

	check_count(tokens, 4, "sphere: expected 'sp <x,y,z> <diameter> <r,g,b>'");
	obj = new_object(OBJ_SPHERE);
	append_object(scene, obj);
	obj->sphere.center = parse_vec3(tokens[1]);
	obj->sphere.radius = parse_double(tokens[2]) / 2.0;
	if (obj->sphere.radius <= 0)
		error_exit("sphere diameter must be positive");
	obj->mat = default_material(parse_color(tokens[3]));
	parse_material(tokens, 3, &obj->mat);
}

void	parse_plane(char **tokens, t_scene *scene)
{
	t_object	*obj;

	check_count(tokens, 4, "plane: expected 'pl <x,y,z> <nx,ny,nz> <r,g,b>'");
	obj = new_object(OBJ_PLANE);
	append_object(scene, obj);
	obj->plane.point = parse_vec3(tokens[1]);
	obj->plane.normal = parse_normal(tokens[2]);
	obj->mat = default_material(parse_color(tokens[3]));
	parse_material(tokens, 3, &obj->mat);
}

void	parse_cylinder(char **tokens, t_scene *scene)
{
	t_object	*obj;

	check_count(tokens, 6, "cylinder: expected "
		"'cy <x,y,z> <ax,ay,az> <diameter> <height> <r,g,b>'");
	obj = new_object(OBJ_CYLINDER);
	append_object(scene, obj);
	obj->cylinder.center = parse_vec3(tokens[1]);
	obj->cylinder.axis = parse_normal(tokens[2]);
	obj->cylinder.radius = parse_double(tokens[3]) / 2.0;
	if (obj->cylinder.radius <= 0)
		error_exit("cylinder diameter must be positive");
	obj->cylinder.height = parse_double(tokens[4]);
	if (obj->cylinder.height <= 0)
		error_exit("cylinder height must be positive");
	obj->cylinder.top_center = vec3_add(obj->cylinder.center,
			vec3_scale(obj->cylinder.axis, obj->cylinder.height / 2.0));
	obj->cylinder.bottom_center = vec3_sub(obj->cylinder.center,
			vec3_scale(obj->cylinder.axis, obj->cylinder.height / 2.0));
	obj->mat = default_material(parse_color(tokens[5]));
	parse_material(tokens, 5, &obj->mat);
}
