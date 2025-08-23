/*
 * bw_mem.c - simple memory write bandwidth benchmark
 *
 * Usage: bw_mem [-P <parallelism>] [-W <warmup>] [-N <repetitions>] size what
 *        what: rd wr rdwr cp fwr frd fcp bzero bcopy
 *
 * Copyright (c) 1994-1996 Larry McVoy.  Distributed under the FSF GPL with
 * additional restriction that results may published only if
 * (1) the benchmark is unmodified, and
 * (2) the version in the sccsid below is included in the report.
 * Support for this development by Sun Microsystems is gratefully acknowledged.
 */
char	*id = "$Id$";

#include "rtthread.h"
#include "hal_base.h"

#define TYPE    int
#define TEST_DATA_LEN    (1024 * 1024 * 1024)

#define uint64 uint64_t
#define iter_t uint32_t

/*
 * rd - 4 byte read, 32 byte stride
 * wr - 4 byte write, 32 byte stride
 * rdwr - 4 byte read followed by 4 byte write to same place, 32 byte stride
 * cp - 4 byte read then 4 byte write to different place, 32 byte stride
 * fwr - write every 4 byte word
 * frd - read every 4 byte word
 * fcp - copy every 4 byte word
 *
 * All tests do 512 byte chunks in a loop.
 *
 * XXX - do a 64bit version of this.
 */
void	rd(iter_t iterations, void *cookie);
void	wr(iter_t iterations, void *cookie);
void	rdwr(iter_t iterations, void *cookie);
void	mcp(iter_t iterations, void *cookie);
void	fwr(iter_t iterations, void *cookie);
void	frd(iter_t iterations, void *cookie);
void	fcp(iter_t iterations, void *cookie);
void	loop_bzero(iter_t iterations, void *cookie);
void	loop_bcopy(iter_t iterations, void *cookie);
void	init_overhead(iter_t iterations, void *cookie);
void	init_loop(iter_t iterations, void *cookie);
void	cleanup(iter_t iterations, void *cookie);

typedef struct _state {
	double	overhead;
	size_t	nbytes;
	int	need_buf2;
	int	aligned;
	TYPE	*buf;
	TYPE	*buf2;
	TYPE	*buf2_orig;
	TYPE	*lastone;
	size_t	N;
} state_t;

void	adjusted_bandwidth(uint64 t, uint64 b, uint64 iter, double ovrhd);

static volatile uint64 g_dummy_result;
void use_int(int result)
{
    g_dummy_result += result;
}

void use_pointer(void *result)
{
    g_dummy_result += (uint32_t)result;
}


int
bw_mem(int ac, char **av)
{
    char *t_case;
	uint32_t addr;
	uint32_t size;
	state_t state;
	iter_t iterations;
	uint64 start, end;
    uint64 time;
	rt_base_t level;
	
	if (ac!= 4) {
		rt_kprintf("usage: bw_mem addr size case(rd, wr, frd, fwr, rdwr, fcp)\n");
		return -1;
	}

	HAL_TIMER_SysTimerInit(TIMER0);
	rt_kprintf("%p: 0x%x\n", &TIMER0->CONTROLREG, TIMER0->CONTROLREG);

	addr = (uint32_t)strtoll(av[1], NULL, 16);
	size = (uint32_t)atoi(av[2]);
	t_case = av[3];
	rt_kprintf("addr=0x%x, size=%dm, case=%s\n", addr, size, t_case);

    size = size * 1024 * 1024;
	state.buf = (TYPE *)addr;
	state.lastone = (TYPE *)(addr + size - 512);
	iterations = TEST_DATA_LEN / size;
	if (TEST_DATA_LEN % size != 0)
		rt_kprintf("size is not pow of 2\n");

    start = end = 0;
	level = rt_hw_interrupt_disable();
	if (!strcmp("rd", t_case)) {
		start = HAL_GetSysTimerCount();
		rd(iterations, &state);
		end = HAL_GetSysTimerCount();
	}
	else if (!strcmp("wr", t_case)) {
		start = HAL_GetSysTimerCount();
		wr(iterations, &state);
		end = HAL_GetSysTimerCount();
	}
	else if (!strcmp("frd", t_case)) {
		start = HAL_GetSysTimerCount();
		frd(iterations, &state);
		end = HAL_GetSysTimerCount();
	}
	else if (!strcmp("fwr", t_case)) {
		start = HAL_GetSysTimerCount();
		fwr(iterations, &state);
		end = HAL_GetSysTimerCount();
	}
	else if (!strcmp("rdwr", t_case)) {
		start = HAL_GetSysTimerCount();
		rdwr(iterations, &state);
		end = HAL_GetSysTimerCount();
	}
	else if (!strcmp("fcp", t_case)) {
		end = HAL_GetSysTimerCount();
		fcp(iterations, &state);
		end = HAL_GetSysTimerCount();
	}
	rt_hw_interrupt_enable(level);
	time = (uint64)(end - start);
	time = (time * 1000000) / PLL_INPUT_OSC_RATE;

	if (time > 0)
		adjusted_bandwidth(time, size, iterations, 0);
	else
		rt_kprintf("time: %d, size: %d, iter: %d\n", time, size, iterations);
	
	return(0);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(bw_mem, memory benchmark);
#endif


void
cleanup(iter_t iterations, void *cookie)
{
	state_t *state = (state_t *) cookie;

	if (iterations) return;

	free(state->buf);
	if (state->buf2_orig) free(state->buf2_orig);
}

void
rd(iter_t iterations, void *cookie)
{	
	state_t *state = (state_t *) cookie;
	register TYPE *lastone = state->lastone;
	register int sum = 0;
	//rt_tick_t start, end;

    //start = rt_tick_get();
	while (iterations-- > 0) {
	    register TYPE *p = state->buf;
	    while (p <= lastone) {
		sum += 
#define	DOIT(i)	p[i]+
		DOIT(0) DOIT(4) DOIT(8) DOIT(12) DOIT(16) DOIT(20) DOIT(24)
		DOIT(28) DOIT(32) DOIT(36) DOIT(40) DOIT(44) DOIT(48) DOIT(52)
		DOIT(56) DOIT(60) DOIT(64) DOIT(68) DOIT(72) DOIT(76)
		DOIT(80) DOIT(84) DOIT(88) DOIT(92) DOIT(96) DOIT(100)
		DOIT(104) DOIT(108) DOIT(112) DOIT(116) DOIT(120) 
		p[124];
		p +=  128;
	    }
	}
	//end = rt_tick_get();
	//rt_kprintf("rd time: %d\n", end - start);
	use_int(sum);
}
#undef	DOIT

void
wr(iter_t iterations, void *cookie)
{	
	state_t *state = (state_t *) cookie;
	register TYPE *lastone = state->lastone;

	while (iterations-- > 0) {
	    register TYPE *p = state->buf;
	    while (p <= lastone) {
#define	DOIT(i)	p[i] = 1;
		DOIT(0) DOIT(4) DOIT(8) DOIT(12) DOIT(16) DOIT(20) DOIT(24)
		DOIT(28) DOIT(32) DOIT(36) DOIT(40) DOIT(44) DOIT(48) DOIT(52)
		DOIT(56) DOIT(60) DOIT(64) DOIT(68) DOIT(72) DOIT(76)
		DOIT(80) DOIT(84) DOIT(88) DOIT(92) DOIT(96) DOIT(100)
		DOIT(104) DOIT(108) DOIT(112) DOIT(116) DOIT(120) DOIT(124);
		p +=  128;
	    }
	}
}
#undef	DOIT

void
rdwr(iter_t iterations, void *cookie)
{	
	state_t *state = (state_t *) cookie;
	register TYPE *lastone = state->lastone;
	register int sum = 0;

	while (iterations-- > 0) {
	    register TYPE *p = state->buf;
	    while (p <= lastone) {
#define	DOIT(i)	sum += p[i]; p[i] = 1;
		DOIT(0) DOIT(4) DOIT(8) DOIT(12) DOIT(16) DOIT(20) DOIT(24)
		DOIT(28) DOIT(32) DOIT(36) DOIT(40) DOIT(44) DOIT(48) DOIT(52)
		DOIT(56) DOIT(60) DOIT(64) DOIT(68) DOIT(72) DOIT(76)
		DOIT(80) DOIT(84) DOIT(88) DOIT(92) DOIT(96) DOIT(100)
		DOIT(104) DOIT(108) DOIT(112) DOIT(116) DOIT(120) DOIT(124);
		p +=  128;
	    }
	}
	use_int(sum);
}
#undef	DOIT

void
mcp(iter_t iterations, void *cookie)
{	
	state_t *state = (state_t *) cookie;
	register TYPE *lastone = state->lastone;
	TYPE* p_save = NULL;

	while (iterations-- > 0) {
	    register TYPE *p = state->buf;
	    register TYPE *dst = state->buf2;
	    while (p <= lastone) {
#define	DOIT(i)	dst[i] = p[i];
		DOIT(0) DOIT(4) DOIT(8) DOIT(12) DOIT(16) DOIT(20) DOIT(24)
		DOIT(28) DOIT(32) DOIT(36) DOIT(40) DOIT(44) DOIT(48) DOIT(52)
		DOIT(56) DOIT(60) DOIT(64) DOIT(68) DOIT(72) DOIT(76)
		DOIT(80) DOIT(84) DOIT(88) DOIT(92) DOIT(96) DOIT(100)
		DOIT(104) DOIT(108) DOIT(112) DOIT(116) DOIT(120) DOIT(124);
		p += 128;
		dst += 128;
	    }
	    p_save = p;
	}
	use_pointer(p_save);
}
#undef	DOIT

void
fwr(iter_t iterations, void *cookie)
{	
	state_t *state = (state_t *) cookie;
	register TYPE *lastone = state->lastone;
	TYPE* p_save = NULL;

	while (iterations-- > 0) {
	    register TYPE *p = state->buf;
	    while (p <= lastone) {
#define	DOIT(i)	p[i]=
		DOIT(0) DOIT(1) DOIT(2) DOIT(3) DOIT(4) DOIT(5) DOIT(6)
		DOIT(7) DOIT(8) DOIT(9) DOIT(10) DOIT(11) DOIT(12)
		DOIT(13) DOIT(14) DOIT(15) DOIT(16) DOIT(17) DOIT(18)
		DOIT(19) DOIT(20) DOIT(21) DOIT(22) DOIT(23) DOIT(24)
		DOIT(25) DOIT(26) DOIT(27) DOIT(28) DOIT(29) DOIT(30)
		DOIT(31) DOIT(32) DOIT(33) DOIT(34) DOIT(35) DOIT(36)
		DOIT(37) DOIT(38) DOIT(39) DOIT(40) DOIT(41) DOIT(42)
		DOIT(43) DOIT(44) DOIT(45) DOIT(46) DOIT(47) DOIT(48)
		DOIT(49) DOIT(50) DOIT(51) DOIT(52) DOIT(53) DOIT(54)
		DOIT(55) DOIT(56) DOIT(57) DOIT(58) DOIT(59) DOIT(60)
		DOIT(61) DOIT(62) DOIT(63) DOIT(64) DOIT(65) DOIT(66)
		DOIT(67) DOIT(68) DOIT(69) DOIT(70) DOIT(71) DOIT(72)
		DOIT(73) DOIT(74) DOIT(75) DOIT(76) DOIT(77) DOIT(78)
		DOIT(79) DOIT(80) DOIT(81) DOIT(82) DOIT(83) DOIT(84)
		DOIT(85) DOIT(86) DOIT(87) DOIT(88) DOIT(89) DOIT(90)
		DOIT(91) DOIT(92) DOIT(93) DOIT(94) DOIT(95) DOIT(96)
		DOIT(97) DOIT(98) DOIT(99) DOIT(100) DOIT(101) DOIT(102)
		DOIT(103) DOIT(104) DOIT(105) DOIT(106) DOIT(107)
		DOIT(108) DOIT(109) DOIT(110) DOIT(111) DOIT(112)
		DOIT(113) DOIT(114) DOIT(115) DOIT(116) DOIT(117)
		DOIT(118) DOIT(119) DOIT(120) DOIT(121) DOIT(122)
		DOIT(123) DOIT(124) DOIT(125) DOIT(126) DOIT(127) 1;
		p += 128;
	    }
	    p_save = p;
	}
	use_pointer(p_save);
}
#undef	DOIT

void
frd(iter_t iterations, void *cookie)
{	
	state_t *state = (state_t *) cookie;
	register int sum = 0;
	register TYPE *lastone = state->lastone;

	while (iterations-- > 0) {
	    register TYPE *p = state->buf;
	    while (p <= lastone) {
		sum +=
#define	DOIT(i)	p[i]+
		DOIT(0) DOIT(1) DOIT(2) DOIT(3) DOIT(4) DOIT(5) DOIT(6)
		DOIT(7) DOIT(8) DOIT(9) DOIT(10) DOIT(11) DOIT(12)
		DOIT(13) DOIT(14) DOIT(15) DOIT(16) DOIT(17) DOIT(18)
		DOIT(19) DOIT(20) DOIT(21) DOIT(22) DOIT(23) DOIT(24)
		DOIT(25) DOIT(26) DOIT(27) DOIT(28) DOIT(29) DOIT(30)
		DOIT(31) DOIT(32) DOIT(33) DOIT(34) DOIT(35) DOIT(36)
		DOIT(37) DOIT(38) DOIT(39) DOIT(40) DOIT(41) DOIT(42)
		DOIT(43) DOIT(44) DOIT(45) DOIT(46) DOIT(47) DOIT(48)
		DOIT(49) DOIT(50) DOIT(51) DOIT(52) DOIT(53) DOIT(54)
		DOIT(55) DOIT(56) DOIT(57) DOIT(58) DOIT(59) DOIT(60)
		DOIT(61) DOIT(62) DOIT(63) DOIT(64) DOIT(65) DOIT(66)
		DOIT(67) DOIT(68) DOIT(69) DOIT(70) DOIT(71) DOIT(72)
		DOIT(73) DOIT(74) DOIT(75) DOIT(76) DOIT(77) DOIT(78)
		DOIT(79) DOIT(80) DOIT(81) DOIT(82) DOIT(83) DOIT(84)
		DOIT(85) DOIT(86) DOIT(87) DOIT(88) DOIT(89) DOIT(90)
		DOIT(91) DOIT(92) DOIT(93) DOIT(94) DOIT(95) DOIT(96)
		DOIT(97) DOIT(98) DOIT(99) DOIT(100) DOIT(101) DOIT(102)
		DOIT(103) DOIT(104) DOIT(105) DOIT(106) DOIT(107)
		DOIT(108) DOIT(109) DOIT(110) DOIT(111) DOIT(112)
		DOIT(113) DOIT(114) DOIT(115) DOIT(116) DOIT(117)
		DOIT(118) DOIT(119) DOIT(120) DOIT(121) DOIT(122)
		DOIT(123) DOIT(124) DOIT(125) DOIT(126) p[127];
		p += 128;
	    }
	}
	use_int(sum);
}
#undef	DOIT

void
fcp(iter_t iterations, void *cookie)
{	
	state_t *state = (state_t *) cookie;
	register TYPE *lastone = state->lastone;

	while (iterations-- > 0) {
	    register TYPE *p = state->buf;
	    register TYPE *dst = state->buf2;
	    while (p <= lastone) {
#define	DOIT(i)	dst[i]=p[i];
		DOIT(0) DOIT(1) DOIT(2) DOIT(3) DOIT(4) DOIT(5) DOIT(6)
		DOIT(7) DOIT(8) DOIT(9) DOIT(10) DOIT(11) DOIT(12)
		DOIT(13) DOIT(14) DOIT(15) DOIT(16) DOIT(17) DOIT(18)
		DOIT(19) DOIT(20) DOIT(21) DOIT(22) DOIT(23) DOIT(24)
		DOIT(25) DOIT(26) DOIT(27) DOIT(28) DOIT(29) DOIT(30)
		DOIT(31) DOIT(32) DOIT(33) DOIT(34) DOIT(35) DOIT(36)
		DOIT(37) DOIT(38) DOIT(39) DOIT(40) DOIT(41) DOIT(42)
		DOIT(43) DOIT(44) DOIT(45) DOIT(46) DOIT(47) DOIT(48)
		DOIT(49) DOIT(50) DOIT(51) DOIT(52) DOIT(53) DOIT(54)
		DOIT(55) DOIT(56) DOIT(57) DOIT(58) DOIT(59) DOIT(60)
		DOIT(61) DOIT(62) DOIT(63) DOIT(64) DOIT(65) DOIT(66)
		DOIT(67) DOIT(68) DOIT(69) DOIT(70) DOIT(71) DOIT(72)
		DOIT(73) DOIT(74) DOIT(75) DOIT(76) DOIT(77) DOIT(78)
		DOIT(79) DOIT(80) DOIT(81) DOIT(82) DOIT(83) DOIT(84)
		DOIT(85) DOIT(86) DOIT(87) DOIT(88) DOIT(89) DOIT(90)
		DOIT(91) DOIT(92) DOIT(93) DOIT(94) DOIT(95) DOIT(96)
		DOIT(97) DOIT(98) DOIT(99) DOIT(100) DOIT(101) DOIT(102)
		DOIT(103) DOIT(104) DOIT(105) DOIT(106) DOIT(107)
		DOIT(108) DOIT(109) DOIT(110) DOIT(111) DOIT(112)
		DOIT(113) DOIT(114) DOIT(115) DOIT(116) DOIT(117)
		DOIT(118) DOIT(119) DOIT(120) DOIT(121) DOIT(122)
		DOIT(123) DOIT(124) DOIT(125) DOIT(126) DOIT(127)
		p += 128;
		dst += 128;
	    }
	}
}

/*
 * Almost like bandwidth() in lib_timing.c, but we need to adjust
 * bandwidth based upon loop overhead.
 */
void adjusted_bandwidth(uint64 time, uint64 bytes, uint64 iter, double overhd)
{
#define MB	(1000. * 1000.)
    char szBuf[64];
	double secs = ((time * 1.0) / iter - overhd) / 1000000.0;
	double mb;
	
    mb = bytes / MB;

	if (secs <= 0.) {
		rt_kprintf("%lld, %lld, %lld\n", time, bytes, iter);
		return;
	}

	
	sprintf(szBuf, "%.2f   %.2f", mb, mb/secs);
	rt_kprintf("%s\n", szBuf);
}


