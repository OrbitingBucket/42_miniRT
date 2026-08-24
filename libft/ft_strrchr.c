/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 06:07:53 by azaytsev          #+#    #+#             */
/*   Updated: 2025/11/14 14:01:09 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	unsigned char	uc;
	size_t			i;
	const char		*last;

	uc = (unsigned char)c;
	i = 0;
	last = NULL;
	while (s[i])
	{
		if ((unsigned char)s[i] == uc)
			last = s + i;
		i++;
	}
	if (uc == '\0')
		return ((char *)(s + i));
	return ((char *)last);
}
