/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alluengo <alluengo@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 13:31:30 by alluengo          #+#    #+#             */
/*   Updated: 2026/09/16 15:53:27 by alluengo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000LL
		+ (long long)tv.tv_usec / 1000LL);
}

long long	elapsed_ms(long long start)
{
	return (get_time_ms() - start);
}

void	ft_usleep(t_sim *sim, long long ms)
{
	long long	start;

	start = get_time_ms();
	while (get_time_ms() - start < ms)
	{
		if (!is_running(sim))
			return ;
		usleep(100);
	}

}

int	ft_atoi_strict(const char *str, long long *out)
{
	int			i;
	long long	result;

	if (!str || !*str)
		return (0);
	if (str[0] == '-')
		return (0);
	i = 0;
	result = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		if (result > (MAX_VALUE - (str[i] - '0')) / 10)
			return (0);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	*out = result;
	return (1);
}
