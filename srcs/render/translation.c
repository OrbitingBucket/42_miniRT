/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   translation.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 16:15:06 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:05 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

t_vec3	get_cam_right(t_vec3 dir)
{
	t_vec3	world_up;

	if (fabs(dir.y) < 0.99)
		world_up = vec3(0, 1, 0);
	else
		world_up = vec3(0, 0, 1);
	return (vec3_norm(vec3_cross(dir, world_up)));
}

void	move_camera(int key, t_camera *cam, double speed)
{
	t_vec3	right;

	right = get_cam_right(cam->dir);
	if (key == 119)
		cam->pos = vec3_add(cam->pos, vec3_scale(cam->dir, speed));
	else if (key == 115)
		cam->pos = vec3_sub(cam->pos, vec3_scale(cam->dir, speed));
	else if (key == 100)
		cam->pos = vec3_add(cam->pos, vec3_scale(right, speed));
	else if (key == 97)
		cam->pos = vec3_sub(cam->pos, vec3_scale(right, speed));
}
