/******************************************************************************
pkstate.h - Global program state

This file is Copyright 2011-2014, The Beanstalks Project ehf.

This program is free software: you can redistribute it and/or modify it under
the terms  of the  Apache  License 2.0  as published by the  Apache  Software
Foundation.

This program is distributed in the hope that it will be useful,  but  WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the Apache License for more details.

You should have received a copy of the Apache License along with this program.
If not, see: <http://www.apache.org/licenses/>

Note: For alternate license terms, see the file COPYING.md.

******************************************************************************/

#define PKS_LOG_DATA_MAX     64*1024

/* Note: This list is replicated in PageKiteAPI.java */
typedef enum {
  PK_STATUS_STARTUP     = 10,
  PK_STATUS_CONNECT     = 20,
  PK_STATUS_DYNDNS      = 30,
  PK_STATUS_FLYING      = 40,
  PK_STATUS_PROBLEMS    = 50,
  PK_STATUS_REJECTED    = 60,
  PK_STATUS_NO_NETWORK  = 90
} pk_status_t;

struct pk_global_state {
  /* Synchronization */
  pthread_mutex_t lock;
  pthread_cond_t  cond;

  /* Global logging state */
  FILE*           log_file;
  unsigned int    log_mask;
  char            log_ring_buffer[PKS_LOG_DATA_MAX+1];
  char*           log_ring_start;
  char*           log_ring_end;

  /* Settings */
  unsigned int    bail_on_errors;
  time_t          conn_eviction_idle_s;
  unsigned int    fake_ping:1;

  /* Global program state */
  unsigned int    live_streams;
  unsigned int    live_tunnels;
  unsigned int    have_ssl:1;
  unsigned int    force_update:1;
  char*           app_id_short;
  char*           app_id_long;

  /* Quota state (assuming frontends agree) */
  int             quota_days;
  int             quota_conns;
  int             quota_mb;
};

#ifdef __IN_PKSTATE_C__
extern struct pk_global_state pk_state;
#else
struct pk_global_state pk_state;
#endif

#define PKS_STATE(change) { pthread_mutex_lock(&(pk_state.lock)); \
                            change; \
                            pthread_cond_broadcast(&(pk_state.cond)); \
                            pthread_mutex_unlock(&(pk_state.lock)); } 

void pks_global_init(unsigned int log_level);
int pks_logcopy(const char*, size_t len);
void pks_copylog(char*);
void pks_printlog(FILE *dest);
