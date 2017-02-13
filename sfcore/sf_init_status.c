#include <sfcore/sf_init_status.h>
#include <sfcore/sf_time.h>
#include <sfcore/sf_logger.h>

SF_CORE_API
void
sf_init_status_init (SfInitStatus *p,
                     int max_init_attempts,
                     int init_delay_ms,
                     int delay_ms,
                     SfInitStatusInitFn init_fn,
                     void *context)
{
  p->max_init_attempts = max_init_attempts;
  p->init_delay_ms = init_delay_ms;
  p->delay_ms = delay_ms;
  p->init_fn = init_fn;
  p->context = context;

  p->open_succeeded = false;
  p->last_try = 0;
  p->num_attempts = 0;
}

SF_CORE_API
void
sf_init_status_destroy (SfInitStatus *p)
{
}

SF_CORE_API
bool
sf_init_status_try (SfInitStatus *p)
{
  const uint64_t now = sf_time_get_tick_count ();

  if (!p->open_succeeded &&
    ((p->num_attempts < p->max_init_attempts && now - p->last_try >= p->init_delay_ms) ||
    now - p->last_try >= p->delay_ms)) {
      p->open_succeeded = (*p->init_fn) (p->context);
      p->last_try = now;
      p->num_attempts++;

      if (!p->open_succeeded) {
        const int delay_ms = p->num_attempts < p->max_init_attempts ?
          p->init_delay_ms : p->delay_ms;

        sf_log_info ("Retrying in %3.2f second(s)...", (float) delay_ms / 1000);
      }
  }

  return p->open_succeeded;
}

SF_CORE_API
void
sf_init_status_reset (SfInitStatus *p)
{
  p->open_succeeded = false;
  p->last_try = 0;
  p->num_attempts = 0;
}
