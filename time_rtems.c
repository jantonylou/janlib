#include <rtems/bsd.h>

uint32_t jtl_milli_uptime32(void)
{
	uint32_t ms;
	struct timeval tv;

	rtems_bsd_microuptime(&tv);

	ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	return ms;
}
