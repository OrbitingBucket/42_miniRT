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

static double	motion_dt(t_app *app, int frame_start)
{
	double	now;
	double	dt;

	now = now_seconds();
	if (now < 0.0)
	{
		if (frame_start)
			return (MOTION_FALLBACK_DT);
		return (0.0);
	}
	dt = now - app->move_clock;
	app->move_clock = now;
	if (dt < 0.0)
		return (0.0);
	if (dt > MOTION_MAX_DT)
		return (MOTION_MAX_DT);
	return (dt);
}

static void	move_held_keys(t_camera *cam, int keys, double dist)
{
	if (keys & MOVE_BIT_W)
		move_camera(KEY_W, cam, dist);
	if (keys & MOVE_BIT_A)
		move_camera(KEY_A, cam, dist);
	if (keys & MOVE_BIT_S)
		move_camera(KEY_S, cam, dist);
	if (keys & MOVE_BIT_D)
		move_camera(KEY_D, cam, dist);
}

void	apply_motion(t_app *app, int frame_start)
{
	int		released;
	double	dt;

	released = app->pending;
	app->pending = 0;
	if (app->is_locked)
	{
		app->keys = 0;
		return ;
	}
	if (app->keys)
	{
		dt = motion_dt(app, frame_start);
		if (dt > 0.0)
		{
			move_held_keys(&app->scene.camera, app->keys, MOVE_SPEED * dt);
			app->needs_render = 1;
		}
	}
	app->keys &= ~released;
}
