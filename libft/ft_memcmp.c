/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 23:46:45 by azaytsev          #+#    #+#             */
/*   Updated: 2025/11/14 13:57:52 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*us1;
	const unsigned char	*us2;
	size_t				nn;

	us1 = (const unsigned char *)s1;
	us2 = (const unsigned char *)s2;
	nn = 0;
	while (nn < n)
	{
		if (us1[nn] != us2[nn])
			return ((int)us1[nn] - (int)us2[nn]);
		nn++;
	}
	return (0);
}
