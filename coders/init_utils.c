/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alluengo <alluengo@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 13:51:13 by alluengo          #+#    #+#             */
/*   Updated: 2026/09/16 15:39:36 by alluengo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_arrays(t_sim *sim, int n)
{
	sim->compile_count = malloc(sizeof(int) * n);
	sim->last_compile_start = malloc(sizeof(long long) * n);
	sim->dongles = malloc(sizeof(t_dongle) * n);
	if (!sim->compile_count || !sim->last_compile_start || !sim->dongles)
		return (0);
	return (1);
}

static void	init_counters(t_sim *sim, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		sim->compile_count[i] = 0;
		sim->last_compile_start[i] = sim->start_time;
		i++;
	}
}

static int	init_dongles(t_sim *sim, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].in_use = 0;
		sim->dongles[i].aviable_at = 0;
		sim->dongles[i].queue = heap_create(n, sim->scheduler);
		sim->nb_dongles_init = i + 1;
		if (!sim->dongles[i].queue)
			return (0);
		i++;
	}
	return (1);
}

int	sim_init(t_sim *sim)
{
	int	n;

	n = sim->nb_coders;
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->running_mutex, NULL);
	sim->running = 1;
	sim->finished = 0;
	if (sim->nb_compiles_required == 0)
	{
		sim->running = 0;
		sim->finished = n;	
	}

	sim->start_time = get_time_ms();
	sim->nb_dongles_init = 0;
	if (!init_arrays(sim, n))
		return (cleanup_sim(sim), 0);
	init_counters(sim, n);
	if (!init_dongles(sim, n))
		return (cleanup_sim(sim), 0);
	return (1);
}

void	cleanup_sim(t_sim *sim)
{
	int	i;

	if (!sim)
		return ;
	i = 0;
	while (i < sim->nb_dongles_init)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		if (sim->dongles[i].queue)
			heap_destroy(sim->dongles[i].queue);
		i++;
	}
	pthread_mutex_destroy(&sim->running_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	free(sim->compile_count);
	free(sim->last_compile_start);
	free(sim->dongles);
}
