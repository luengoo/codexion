/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alluengo <alluengo@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 13:47:49 by alluengo          #+#    #+#             */
/*   Updated: 2026/09/16 15:44:07 by alluengo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_all_done(t_sim *sim)
{
	int	done;

	pthread_mutex_lock(&sim->running_mutex);
	done = (sim->finished >= sim->nb_coders);
	pthread_mutex_unlock(&sim->running_mutex);
	return (done);
}

static void	stop_sim(t_sim *sim)
{
	pthread_mutex_lock(&sim->running_mutex);
	sim->running = 0;
	pthread_mutex_unlock(&sim->running_mutex);
}

void	mark_done(t_sim *sim)
{
	pthread_mutex_lock(&sim->running_mutex);
	sim->finished++;
	pthread_mutex_unlock(&sim->running_mutex);
}

static int	check_burnout_loop(t_sim *sim, long long now)
{
	int			i;
	long long	since_last;

	i = 0;
	pthread_mutex_lock(&sim->running_mutex);
	while (i < sim->nb_coders)
	{
		if (sim->compile_count[i] < sim->nb_compiles_required)
		{
			since_last = now - sim->last_compile_start[i];
			if (since_last >= sim->time_to_burnout)
			{
				pthread_mutex_unlock(&sim->running_mutex);
				return (ft_log_burnout(sim, i + 1), 1);
			}
		}
		i++;
	}
	pthread_mutex_unlock(&sim->running_mutex);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;
	long long	now;

	sim = (t_sim *)arg;
	while (1)
	{
		usleep(1000);
		if (!is_running(sim))
			break ;
		now = get_time_ms();
		if (check_burnout_loop(sim, now))
			return (NULL);
		if (check_all_done(sim))
		{
			stop_sim(sim);
			break ;
		}
	}
	return (NULL);
}
