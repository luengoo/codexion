#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

# define FIFO 0
# define EDF 1

typedef struct s_request
{
    int             coder_id;
    long long       timestamp;
    long long       deadline;
    int             granted;
    pthread_cond_t  cond; 
}   t_request;

typedef struct s_heap
{
    t_request   **data;
    int         size;
    int         capacity;
    int         mode;
}   t_heap;

typedef struct  s_dongle
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    t_heap          *queue;
    int             in_use;
    long long       aviable_at;
}   t_dongle;

typedef struct s_sim
{
    int             nb_coders;
    long long       time_to_burnout;
    long long       time_to_compile;
    long long       time_to_debug;
    long long       time_to_refractor;
    long long       nb_compiles_required;
    long long       dongle_cooldown;
    int             scheduler;
    pthread_mutex_t log_mutex;
    pthread_mutex_t running_mutex;
    int             running;
    long long       start_time;
    t_dongle        *dongles;
    int             *compile_count;
    long long       *last_compile_start;
}   t_sim;

typedef struct  s_coder
{
    int     id;
    t_sim   *sim;
}   t_coder;


/*parser.c*/
int         arg_parser(int argc, char **argv, t_sim *sim);

/*init_utils.c*/
int         sim_init(t_sim *sim);
void        cleanup_sim(t_sim *sim);

/*scheduler.c*/
int         scheduler_compare(t_heap *h, t_request *a, t_request *b);
t_heap      *heap_create(int capacity, int mode);
void        heap_destroy(t_heap *h);

/*log.c*/
void        ft_log(t_sim *sim, int coder_id, const char *msg);
void        ft_log_burnout(t_sim *sim, int coder_id);

/*utils.c*/
long long   get_time_ms(void);
long long   elapsed_ms(long long start);
void        ft_usleep(long long ms);
int	        ft_atoi_strict(const char *str, long long *out);

/*scheduler_utils.c*/
void        heap_push(t_heap *h, t_request *req);
t_request   *heap_pop(t_heap *h);

/*coder.c*/
int         is_running(t_sim *sim);
void        *coder_routine(void *arg);
int         coder_compile(t_coder *c);

/*coder_utils.c*/
void        init_requests(t_coder *c, t_request *l, t_request *r);
int         one_coder_case(t_sim *sim, t_request *l);
void        do_compile(t_coder *c);
int         take_dongles_normal(t_coder *c, t_request *l, t_request *r);
int         take_dongles_last(t_coder *c, t_request *l, t_request *r);

/*dongle.c*/
void        dongle_acquire(t_sim *sim, int dongle_idx, t_request *req);
void        dongle_release(t_sim *sim, int dongle_idx);

/*monitor.c*/
void        *monitor_routine(void *arg);
int         check_all_done(t_sim *sim);



#endif