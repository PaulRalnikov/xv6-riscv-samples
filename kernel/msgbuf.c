#include <stdarg.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"

char msg_buf[BUFMSGSZ];
int buf_start;
int buf_end;

struct spinlock msg_buf_lock;

int add_buf(int i) {
	return (i == BUFMSGSZ - 1 ? 0 : i + 1);
}

void initmsgbuf() {
	buf_start = 0;
	msg_buf[0] = '\n';
	buf_end = 1;
	initlock(&msg_buf_lock, "message buffer lock");
}

// msg_buf_lock must be held!
void put_char_to_buf(char c) {
	if (buf_end == buf_start) {
		buf_start = add_buf(buf_start);
	}
	msg_buf[buf_end] = c;
	buf_end = add_buf(buf_end);
}

static char digits_for_buf[] = "0123456789abcdef";

// msg_buf_lock must be held!
void put_int_to_buf(int xx, int base, int sign) {
	char buf[16];
    int i;
    uint x;

    if(sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;

    i = 0;
    do {
        buf[i++] = digits_for_buf[x % base];
    } while((x /= base) != 0);

    if(sign)
        buf[i++] = '-';

    while(--i >= 0)
        put_char_to_buf(buf[i]);
}

// msg_buf_lock must be held!
void put_ptr_to_buf(uint64 x)
{
    int i;
    put_char_to_buf('0');
    put_char_to_buf('x');
    for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
        put_char_to_buf(digits_for_buf[x >> (sizeof(uint64) * 8 - 4)]);
}

void pr_msg(const char* fmt, ...) {
	int t = ticks;
	va_list ap;

	//TODO: fix if
	if (fmt == 0) return;
	acquire(&msg_buf_lock);

	put_char_to_buf('[');
	put_int_to_buf(t, 10, 1);
	put_char_to_buf(']');
	put_char_to_buf(' ');

	va_start(ap, fmt);
	char c;
	char* s;
	for (int i = 0; fmt[i] != 0; i++) {
		c = fmt[i];
		if (c != '%') {
			put_char_to_buf(c);
			continue;
		}
		c = fmt[++i];
		if (c == 0) break;
		switch (c) {
		case 'd':
			put_int_to_buf(va_arg(ap, int), 10, 1);
      		break;
      	case 'x':
      		put_int_to_buf(va_arg(ap, int), 16, 1);
      		break;
      	case 'p':
	        put_ptr_to_buf(va_arg(ap, uint64));
	        break;
	    case 's':
	        if((s = va_arg(ap, char*)) == 0)
	            s = "(null)";
	        for(; *s; s++)
	            put_char_to_buf(*s);
	        break;
        case '%':
            put_char_to_buf('%');
            break;
	    default:
	        put_char_to_buf('%');
	        put_char_to_buf(c);
	        break;
		}
	}
	va_end(ap);
	put_char_to_buf('\n');
	release(&msg_buf_lock);
}

uint64 sys_dmesg(void) {
	uint64 addr_buf;
	int sz;

	argaddr(0, &addr_buf);
	char* user_buf = (char*) addr_buf;
	argint(1, &sz);

	if (sz < 2) return -2;

	acquire(&msg_buf_lock);

	int i = buf_start;
	while (msg_buf[i] != '\n') i = add_buf(i);
	buf_start = i;
	i = add_buf(i);
	char zero = 0;
	int j = 0;
	int to_end_sz = (i < buf_end ? buf_end - i : BUFMSGSZ - i);
	pagetable_t pagetable = myproc()->pagetable;

	if (sz - 1 <= to_end_sz) {
		if (copyout(pagetable, (uint64)(user_buf + j), msg_buf + i, sizeof(char) * (sz - 1)) != 0) {
			release(&msg_buf_lock);
			return -1;
		}
		j += sz - 1;
		if (copyout(pagetable, (uint64)(user_buf + j), &zero, sizeof(char)) != 0) {
			release(&msg_buf_lock);
			return -1;
		}
		release(&msg_buf_lock);
		return 0;
	}
	
	if (copyout(pagetable, (uint64)(user_buf + j), msg_buf + i, sizeof(char) * to_end_sz) != 0) {
		release(&msg_buf_lock);
		return -1;
	}

	i += to_end_sz;
	if (i == BUFMSGSZ) i = 0;
	j += to_end_sz;
	sz -= to_end_sz;
	
	if (i == buf_end) {
		if (copyout(pagetable, (uint64)(user_buf + j), &zero, sizeof(char)) != 0) {
			release(&msg_buf_lock);
			return -1;
		}
		release(&msg_buf_lock);
		return 0;
	}

	int copy_sz = buf_end - i;
	if (sz - 1 < copy_sz) copy_sz = sz - 1;

	if (copyout(pagetable, (uint64)(user_buf + j), msg_buf + i, sizeof(char) * copy_sz) != 0) {
		release(&msg_buf_lock);
		return -1;
	}
	release(&msg_buf_lock);

	j += copy_sz;

	if (copyout(pagetable, (uint64)(user_buf + j), &zero, sizeof(char)) != 0) {
		return -1;
	}
	
	return 0;
}