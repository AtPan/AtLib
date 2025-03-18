#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "Atlib/io/bufread.h"
#include "Atlib/error.h"
#include "Atlib/io/bufwrite.h"

#define UBYTE_MASK 0x00ff

static usize __fill(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    isize i = self->to_read;
    if(ferror(self->fh) || feof(self->fh)) return 0; /* Don't read on errors */

    memcpy(self->buf, self->next, i);

    self->next = self->buf; /* Reset next pointer */
    if(((self->to_read = fread(self->buf + i, 1, __ATLIB_BUFREAD_SIZE - i, self->fh)) < __ATLIB_BUFREAD_SIZE - i 
                && ferror(self->fh)) || self->to_read == 0) { 
        return 0; 
    }
    self->to_read += i;   /* Add existing bytes to what was read */
    return self->to_read; /* Return bytes to be read */
}

bufread_t * atlib_bufread_open(bufread_t * restrict self, const char * restrict file_path) {
    atlib_compassert(self);

    FILE * fh = fopen(file_path, "w+");
    if(fh == NULL) {
        return NULL;
    }
    self->fh = fh;
    self->attach = 0;
    self->to_read = 0;
    self->next = self->buf;
    //if(__fill(self) == 0) return NULL;

    return self;
}

bufwrite_t * atlib_bufread_to_bufwrite(const bufread_t * restrict self, bufwrite_t * restrict new) {
    atlib_compassert(self);
    atlib_compassert(new);
    return atlib_bufwrite_fopen(new, self->fh);
}

bufread_t * atlib_bufread_fopen(bufread_t * restrict self, FILE * restrict file) {
    atlib_compassert(self);

    self->fh = file;
    self->attach = 1;
    self->to_read = 0;
    self->next = self->buf;
    //if(__fill(self) == 0) return NULL;
    return self;
}

void atlib_bufread_close(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(!self->attach) fclose(self->fh);
}

isize atlib_bufread_scan(bufread_t * restrict self, const char * restrict fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    __fill(self);
    isize v = vsscanf(self->next, fmt, arg);
    va_end(arg);
    return v;
}

isize atlib_bufread_read_nline(bufread_t * restrict self, void * restrict b, isize n) {
    atlib_compassert(self);
    atlib_compassert(b);
    atlib_compassert(n > 0);

    char * buf = (char *)b;
    const isize r = n;
    char c = 1;

    while(n && c != '\n' && (self->to_read > 0 || __fill(self) > 0)) {
        c = *buf++ = *self->next++;
        self->to_read--;
        n--;
    }
    *(--buf) = 0;

    return r - n;
}

isize atlib_bufread_readn(bufread_t * restrict self, void * restrict b, isize n) {
    atlib_compassert(self);
    atlib_compassert(b);
    atlib_compassert(n > 0);

    char * buf = (char *)b;
    const isize r = n;
    char c = 1;
    while(n && c && (self->to_read > 0 || __fill(self) > 0)) {
        c = *buf++ = *self->next++;
        self->to_read--;
        n--;
    }
    *(--buf) = 0;

    return r - n;
}

u8 atlib_bufread_read_u8(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read <= 0 && __fill(self) == 0) return 0;
    self->to_read--;
    return *self->next++;
}

u16 atlib_bufread_read_u16(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 2 && __fill(self) < 2) {
        return 0;
    }
    self->to_read -= 2;
    u16 ret = *self->next++ & UBYTE_MASK;
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

u32 atlib_bufread_read_u32(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    atlib_assert(self->to_read >= 4 || __fill(self) >= 4);
    //if(self->to_read < 4 && __fill(self) < 4) return 0;
    self->to_read -= 4;
    u32 ret = *self->next++ & UBYTE_MASK;
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

u64 atlib_bufread_read_u64(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 8 && __fill(self) < 8) return 0;

    self->to_read -= 8;
    u64 ret = *self->next++ & UBYTE_MASK;
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

i8 atlib_bufread_read_i8(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 1 && __fill(self) < 1) return 0;
    self->to_read--;
    return *self->next++;
}

i16 atlib_bufread_read_i16(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 2 && __fill(self) < 2) return 0;
    self->to_read -= 2;
    u16 ret = *self->next++;
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

i32 atlib_bufread_read_i32(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    atlib_assert(self->to_read >= 4 || __fill(self) >= 4);
    //if(self->to_read < 4 && __fill(self) < 4) return 0;

    self->to_read -= 4;
    i32 ret = *self->next++;
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

i64 atlib_bufread_read_i64(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 8 && __fill(self) < 8) return 0;

    self->to_read -= 8;
    i64 ret = *self->next++;
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

void atlib_bufread_skip(bufread_t * self, isize n) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(ferror(self->fh)) return;

    if(self->to_read >= n) {
        self->to_read -= n;
        self->next += n;
        return;
    }

    if(!fseek(self->fh, n, SEEK_CUR)) return;

    self->to_read = 0;
    self->next = self->buf;
}

void atlib_bufread_rewind(bufread_t * self, usize n) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(ferror(self->fh)) return;

    if((usize)self->next - (usize)self->buf >= n) {
        self->to_read += n;
        self->next -= n;
        return;
    }

    if(fseek(self->fh, -1 * n, SEEK_CUR)) return;

    self->to_read = 0;
    self->next = self->buf;
}

void atlib_bufread_seek(bufread_t * self, usize n) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(ferror(self->fh) || fseek(self->fh, n, SEEK_SET)) return;
    self->to_read = 0;
    self->next = self->buf;
}
