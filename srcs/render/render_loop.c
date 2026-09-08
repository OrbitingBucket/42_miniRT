/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_loop.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 22:00:00 by azaytsev          #+#    #+#             */
/*   Updated: 2026/09/07 22:00:00 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static void	start_frame(t_app *app)
{
	app->render_camera = app->scene.camera;
	app->needs_render = 0;
	app->fast = 1;
	app->row = 0;
	app->preview_row = 0;
}

static void	render_slice(t_app *app)
{
	int	end;

	end = app->row + SLICE_ROWS;
	if (!app->fast)
		end = app->row + QUALITY_SLICE_ROWS;
	if (end > HEIGHT)
		end = HEIGHT;
	render_span(app, app->row, end);
	app->row = end;
	if (!app->fast && app->row >= HEIGHT)
		present_frame(app);
}

static void	preview_slice(t_app *app)
{
	int	end;

	end = app->preview_row + SLICE_ROWS;
	if (end > HEIGHT)
		end = HEIGHT;
	upsample_span(app, app->preview_row, end);
	app->preview_row = end;
	if (app->preview_row >= HEIGHT)
		present_frame(app);
}

int	loop_hook(t_app *app)
{
	if (app->needs_render && (!app->fast || app->preview_row >= HEIGHT))
		start_frame(app);
	if (app->row < HEIGHT)
		render_slice(app);
	else if (app->fast && app->preview_row < HEIGHT)
		preview_slice(app);
	return (0);
}
