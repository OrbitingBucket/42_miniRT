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

double	now_seconds(void)
{
	char	buf[64];
	int		fd;
	int		len;

	fd = open("/proc/uptime", O_RDONLY);
	if (fd < 0)
		return (-1.0);
	len = read(fd, buf, sizeof(buf) - 1);
	close(fd);
	if (len <= 0)
		return (-1.0);
	buf[len] = '\0';
	return (ft_strtod(buf, NULL));
}
