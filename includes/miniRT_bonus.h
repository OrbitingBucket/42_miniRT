/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miniRT_bonus.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 10:00:00 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:01 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_BONUS_H
# define MINIRT_BONUS_H

# include "miniRT.h"

void				parse_cone(char **tokens, t_scene *scene);
void				parse_triangle(char **tokens, t_scene *scene);
int					intersect_cone(t_ray ray, t_object *obj, t_hit *hit);
int					intersect_triangle(t_ray ray, t_object *obj, t_hit *hit);
void				cone_quad(t_ray ray, t_object *obj, t_quad *q);
t_vec3				cone_body_normal(t_object *obj, t_vec3 point, double proj);
int					check_cone_body(t_ray ray, t_object *obj, t_hit *hit,
						t_quad *q);

t_color				specular_light(t_hit *hit, t_light *light, t_ray view_ray);
void				apply_checker(t_hit *hit, t_material *tmp);

#endif
