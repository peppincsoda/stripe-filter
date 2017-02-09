#ifndef _SF_INIT_STATUS_H_
#define _SF_INIT_STATUS_H_

#include "sf_bool.h"

#include <stdint.h>

/*
   `SfInitStatus` is useful to make sure that a resource is open before accessing it.
   On each call, `sf_init_status_try` calls the `init_fn` member of the structure until it returns true.
   Usage pattern:
 
   if (sf_init_status_try (&status)) { // Tries to open the resource if it is not already open
     // Resource is accessible
     if (!do_sg_with_resource ()) {
        // Resource failed
        close_resource ();
        sf_init_status_reset (&status); // The next `try` call will try to open the resource again
     }
   }
 */

typedef bool (*SfInitStatusInitFn) (void *context);

typedef struct _SfInitStatus SfInitStatus;

struct _SfInitStatus
{
  int max_init_attempts;        // Number of initial attempts of calling `init_fn`.
  int init_delay_ms;            // Delay between the initial attempts.
  int delay_ms;                 // Delay between subsequent `init_fn` calls after the initial attempts failed.
  SfInitStatusInitFn init_fn;   // The function to call on each attempt.
  void *context;                // Context parameter passed to `init_fn`.

  bool open_succeeded;          // True if `init_fn` returned true.
  uint64_t last_try;            // The timestamp of the last try.
  int num_attempts;             // The number of attempts so far.
};

void
sf_init_status_init (SfInitStatus *p,
                     int max_init_attempts,
                     int init_delay_ms,
                     int delay_ms,
                     SfInitStatusInitFn init_fn,
                     void *context);

void
sf_init_status_destroy (SfInitStatus *p);

/* Calls `init_fn` if it have not succeeded yet and the delay times have expired.
   Returns true if `init_fn` has succeeded. */
bool
sf_init_status_try (SfInitStatus *p);

/* Call `sf_init_status_reset` if your resource failed and you want to reopen it.
   The next `sf_init_status_try` will call `init_fn` again and again until it succeeds. */
void
sf_init_status_reset (SfInitStatus *p);

#endif  //  _SF_INIT_STATUS_H_
