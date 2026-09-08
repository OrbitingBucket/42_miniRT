/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clock.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 07:00:00 by azaytsev          #+#    #+#             */
/*   Updated: 2026/09/08 07:00:00 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"
#include <sys/time.h>

double	now_seconds(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) < 0)
		return (-1.0);
	return (tv.tv_sec + tv.tv_usec / 1e6);
}
