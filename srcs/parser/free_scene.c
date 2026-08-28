/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_scene.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 14:13:11 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:03 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

void	free_scene(t_scene *scene)
{
	t_object	*obj;
	t_object	*next_obj;
	t_light		*light;
	t_light		*next_light;

	obj = scene->objects;
	while (obj)
	{
		next_obj = obj->next;
		free(obj->mat.bump);
		free(obj);
		obj = next_obj;
	}
	light = scene->lights;
	while (light)
	{
		next_light = light->next;
		free(light);
		light = next_light;
	}
}

t_cleanup	*cleanup_slot(void)
{
	static t_cleanup	slot;

	return (&slot);
}

void	cleanup_all(void)
{
	t_cleanup	*cl;

	cl = cleanup_slot();
	if (cl->tokens)
		free_tokens(cl->tokens);
	cl->tokens = NULL;
	if (cl->scene)
		free_scene(cl->scene);
	cl->scene = NULL;
	if (cl->fd > 2)
		close(cl->fd);
	cl->fd = 0;
}
