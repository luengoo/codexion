/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alluengo <alluengo@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 13:46:06 by alluengo          #+#    #+#             */
/*   Updated: 2026/09/12 18:03:43 by alluengo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_scheduler(char *argv, t_sim *sim)
{
	if (strcmp(argv, "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(argv, "edf") == 0)
		sim->scheduler = EDF;
	else
		return (0);
	return (1);
}

static int	value_parser(char **argv, long long *vals)
{
	int	i;

	i = 0;
	while (i < 7)
	{
		if (!ft_atoi_strict(argv[i + 1], &vals[i]))
			return (fprintf(stderr,
					"Error: invalid argument '%s'\n", argv[i + 1]), 0);
		i++;
	}
	return (1);
}

static int	check_args(long long *vals, char **argv, t_sim *sim)
{
	if (vals[0] < 1 || vals[0] > MAX_CODERS)
		return (fprintf(stderr,
				"Error: number_of_coders must be between 1 and %d\n",
				MAX_CODERS), 0);
	if (!check_scheduler(argv[8], sim))
		return (fprintf(stderr,
				"Error: scheduler must be 'fifo' or 'edf'\n"), 0);
	return (1);
}

static void	sim_fill(t_sim *sim, long long *vals)
{
	sim->nb_coders = (int)vals[0];
	sim->time_to_burnout = vals[1];
	sim->time_to_compile = vals[2];
	sim->time_to_debug = vals[3];
	sim->time_to_refactor = vals[4];
	sim->nb_compiles_required = (int)vals[5];
	sim->dongle_cooldown = vals[6];
}

int	arg_parser(int argc, char **argv, t_sim *sim)
{
	long long	vals[7];

	if (argc != 9)
		return (fprintf(stderr,
				"Usage: %s n_coders t_burnout t_compile "
				"t_debug t_refactor n_compiles "
				"dongle_cooldown scheduler\n", argv[0]), 0);
	if (!value_parser(argv, vals))
		return (0);
	if (!check_args(vals, argv, sim))
		return (0);
	sim_fill(sim, vals);
	return (1);
}
