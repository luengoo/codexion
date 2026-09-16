/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alluengo <alluengo@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 13:59:59 by alluengo          #+#    #+#             */
/*   Updated: 2026/09/16 15:56:02 by alluengo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_running(t_sim *sim)
{
	int	val;

	pthread_mutex_lock(&sim->running_mutex);
	val = sim->running;
	pthread_mutex_unlock(&sim->running_mutex);
	return (val);
}

void	*coder_routine(void *arg)
{
	t_coder	*c;
	t_sim	*sim;

	c = (t_coder *)arg;
	sim = c->sim;
	ft_usleep(sim, (c->id - 1) * 2);
	while (is_running(sim))
	{
		if (!coder_compile(c))
			break ;
		if (!is_running(sim))
			break ;
		ft_log(sim, c->id, "is debugging");
		ft_usleep(sim, sim->time_to_debug);
		if (!is_running(sim))
			break ;
		ft_log(sim, c->id, "is refactoring");
		ft_usleep(sim, sim->time_to_refactor);
		if (sim->compile_count[c->id - 1] >= sim->nb_compiles_required)
		{
			mark_done(sim);
			break ;
		}
	}
	return (NULL);
}

static void	destroy_requests(t_request *l, t_request *r)
{
	pthread_cond_destroy(&l->cond);
	pthread_cond_destroy(&r->cond);
}

static int	acquire_dongles(t_coder *c, t_request *l, t_request *r)
{
	t_sim	*sim;

	sim = c->sim;
	if (c->id < sim->nb_coders)
		return (take_dongles_normal(c, l, r));
	return (take_dongles_last(c, l, r));
}

int	coder_compile(t_coder *c)
{
	t_sim		*sim;
	t_request	l;
	t_request	r;
	int			ok;

	sim = c->sim;
	init_requests(c, &l, &r);
	if (sim->nb_coders == 1)
		ok = one_coder_case(sim, &l);
	else if (!acquire_dongles(c, &l, &r))
		ok = 0;
	else
	{
		do_compile(c);
		dongle_release(sim, c->id % sim->nb_coders);
		dongle_release(sim, c->id - 1);
		ok = 1;
	}
	destroy_requests(&l, &r);
	return (ok);
}
