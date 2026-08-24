/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_objects_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 10:00:00 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:04 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

void	parse_cone(char **tokens, t_scene *scene)
{
	t_object	*obj;

	if (token_count(tokens) < 6)
		error_exit("cone: 'co <apex> <axis> <radius> <height> <color>'");
	obj = new_object(OBJ_CONE);
	obj->cone.apex = parse_vec3(tokens[1]);
	obj->cone.axis = parse_normal(tokens[2]);
	obj->cone.radius = parse_double(tokens[3]);
	if (obj->cone.radius <= 0)
		error_exit("cone radius must be positive");
	obj->cone.height = parse_double(tokens[4]);
	if (obj->cone.height <= 0)
		error_exit("cone height must be positive");
	obj->mat = default_material(parse_color(tokens[5]));
	parse_material(tokens, 5, &obj->mat);
	append_object(scene, obj);
}

void	parse_triangle(char **tokens, t_scene *scene)
{
	t_object	*obj;
	t_vec3		e1;
	t_vec3		e2;

	if (token_count(tokens) < 5)
		error_exit("triangle: 'tr <v0> <v1> <v2> <color>'");
	obj = new_object(OBJ_TRIANGLE);
	obj->triangle.v0 = parse_vec3(tokens[1]);
	obj->triangle.v1 = parse_vec3(tokens[2]);
	obj->triangle.v2 = parse_vec3(tokens[3]);
	e1 = vec3_sub(obj->triangle.v1, obj->triangle.v0);
	e2 = vec3_sub(obj->triangle.v2, obj->triangle.v0);
	obj->triangle.normal = vec3_norm(vec3_cross(e1, e2));
	if (vec3_near_zero(obj->triangle.normal))
		error_exit("triangle: degenerate (vertices are collinear)");
	obj->mat = default_material(parse_color(tokens[4]));
	parse_material(tokens, 4, &obj->mat);
	append_object(scene, obj);
}
