#include <stdio.h>
#include <string.h>

#include "Atlib/io/bufread.h"
#include "Atlib/error.h"
#include "Atlib/io/bufread_flags.h"

#define UBYTE_MASK 0x00ff

static isize __fill(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    i32 i = self->to_read;
    //if(ferror(self->fh) || feof(self->fh)) return 0; // Don't read on errors

    memmove(self->buf, self->next, i);

    self->next = self->buf; // Reset next pointer
    if(((self->to_read = fread(self->buf + i, 1, __ATLIB_BUFREAD_SIZE - i, self->fh)) < __ATLIB_BUFREAD_SIZE - i 
                && ferror(self->fh)) || self->to_read == 0) { 
        return 0; 
    }
    self->to_read += i;   // Add existing bytes to what was read
    return self->to_read; // Return bytes to be read
}

bufread_t * atlib_bufread_open(bufread_t * restrict self, const char * restrict file_path, u32 br_flags) {
    atlib_compassert(self);
    atlib_compassert(file_path);

    FILE * fh = fopen(file_path, "r");
    if(fh == NULL) {
        return NULL;
    }
    self->fh = fh;
    self->flags = br_flags == 0 ? BUFREAD_FLAG_DEFAULT : br_flags;
    self->to_read = 0;
    self->next = self->buf;

    return self;
}

bufread_t * atlib_bufread_fopen(bufread_t * restrict self, FILE * restrict file) {
    atlib_compassert(self);

    self->fh = file;
    self->flags = BUFREAD_FLAG_DEFAULT | BUFREAD_FH_ATTACH;
    self->to_read = 0;
    self->next = self->buf;
    return self;
}

void atlib_bufread_close(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    self->next = nullptr;
    self->to_read = 0;
    if(~self->flags & BUFREAD_FH_ATTACH) fclose(self->fh);
    memset(self->buf, 0, sizeof(self->buf));
}

isize atlib_bufread_read_nline(bufread_t * restrict self, void * restrict b, u32 n) {
    atlib_compassert(self);
    atlib_compassert(b);

    char * buf = (char *)b;
    const u32 r = n;
    char c = 1;

    while(n && c != '\n' && (self->to_read > 0 || __fill(self) > 0)) {
        c = *buf++ = *self->next++;
        self->to_read--;
        n--;
    }
    *(--buf) = 0;

    return r - n;
}

u32 atlib_bufread_read(bufread_t * const restrict self, void * const restrict b, const u32 blk, const u32 n) {
    atlib_compassert(self);
    atlib_compassert(b);
    atlib_compassert((usize)b < (usize)self || (usize)b >= (usize)self + sizeof(bufread_t));

    char * const buf = (char *)b;
    const isize rb = blk * n;
    isize rem_bytes = blk * n;

    // While there is more bytes requested than in buffer...
    while(rem_bytes > __ATLIB_BUFREAD_SIZE && __fill(self)) {
        memcpy(&buf[rb - rem_bytes], self->buf, __ATLIB_BUFREAD_SIZE);
        rem_bytes -= __ATLIB_BUFREAD_SIZE;
        self->to_read = 0;
    }

    if(rem_bytes <= self->to_read || rem_bytes <= __fill(self)) {
        memcpy(&buf[rb - rem_bytes], self->next, rem_bytes);
        self->next += rem_bytes;
        self->to_read -= rem_bytes;
        rem_bytes = 0;
    }

    return n - (rem_bytes / blk);

    /*
    char * const buf = (char *)b;
    const usize blks_per_buf = __ATLIB_BUFREAD_SIZE / blk;
    const usize blks_per_buf_bytes = blks_per_buf * blk;
    const usize limit = blk * n;
    const usize r = n;

    // While there are enough blks to save that take about whole buffer
    while(n >= blks_per_buf && ((usize)self->to_read >= blks_per_buf_bytes || __fill(self) >= blks_per_buf_bytes)) {
        memcpy(&buf[limit - (n * blk)], self->next, blks_per_buf_bytes);
        self->to_read -= blks_per_buf_bytes;
        n -= blks_per_buf;
        __fill(self);
    }

    const usize rem = blk * n;
    if(((usize)self->to_read >= rem || __fill(self) >= rem)) {
        memcpy(&buf[limit - rem], self->buf, rem);
        self->to_read -= rem;
        self->next += rem;
        n = 0;
    }
       
    return r - n;
    */
}

isize atlib_bufread_readn(bufread_t * restrict self, void * restrict b, u32 n) {
    atlib_compassert(self);
    atlib_compassert(b);

    const isize r = n;
    char * buf = (char *)b;

    if(n <= self->to_read || n <= __fill(self)) {
        memcpy(b, self->next, n);
        self->to_read -= n;
        self->next += n;
        return n;
    }

    do {
        memcpy(&buf[r - n], self->next, self->to_read);
        n -= self->to_read;
        self->to_read = 0;
    } while(__fill(self) && self->to_read < n);

    // Only fails if __fill fails
    if(self->to_read >= n) {
        memcpy(&buf[r - n], self->next, n);
        self->to_read -= n;
        self->next += n;
        n = 0;
    }
    return r - n;

    //Legacy func that does works, in case rework fails down the line
    //while(n && (self->to_read > 0 || __fill(self) > 0)) {
    //    *buf++ = *self->next++;
    //    self->to_read--;
    //    n--;
    //}

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
    atlib_compassert(self->next);

    if(self->to_read < 2 && __fill(self) < 2) {
        return 0;
    }
    self->to_read -= 2;
    u16 ret = *self->next++ & UBYTE_MASK;

    if(self->flags & BUFREAD_READ_BE) {
        return (ret << 8) | (*self->next++ & UBYTE_MASK);
    }
    else {
        return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    }
}

u16 atlib_bufread_read_u16_be(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);
    atlib_compassert(self->next);

    if(self->to_read < 2 && __fill(self) < 2) {
        return 0;
    }
    self->to_read -= 2;
    u16 ret = *self->next++ & UBYTE_MASK;
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

u16 atlib_bufread_read_u16_le(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);
    atlib_compassert(self->next);

    if(self->to_read < 2 && __fill(self) < 2) {
        return 0;
    }
    self->to_read -= 2;
    u16 ret = *self->next++ & UBYTE_MASK;
    return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
}

u32 atlib_bufread_read_u32(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);
    atlib_compassert(self->next);

    if(self->to_read < 4 && __fill(self) < 4) {
        return 0;
    }
    self->to_read -= 4;
    u32 ret = *self->next++ & UBYTE_MASK;

    if(self->flags & BUFREAD_READ_BE) {
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        return (ret << 8) | (*self->next++ & UBYTE_MASK);
    }
    else {
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    }
}

u32 atlib_bufread_read_u32_be(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 4 && __fill(self) < 4) return 0;
    self->to_read -= 4;
    u32 ret = *self->next++ & UBYTE_MASK;
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

u32 atlib_bufread_read_u32_le(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 4 && __fill(self) < 4) return 0;
    self->to_read -= 4;
    u32 ret = *self->next++ & UBYTE_MASK;
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 16);
    return (ret) | ((*self->next++ & UBYTE_MASK) << 24);
}

u64 atlib_bufread_read_u64(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);
    atlib_compassert(self->next);

    if(self->to_read < 8 && __fill(self) < 8) {
        return 0;
    }
    self->to_read -= 8;
    u64 ret = *self->next++ & UBYTE_MASK;

    if(self->flags & BUFREAD_READ_BE) {
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        return (ret << 8) | (*self->next++ & UBYTE_MASK);
    }
    else {
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    }

    if(self->flags & BUFREAD_READ_BE) return atlib_bufread_read_u64_be(self);
    else return atlib_bufread_read_u64_le(self);
}

u64 atlib_bufread_read_u64_be(bufread_t * self) {
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

u64 atlib_bufread_read_u64_le(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 8 && __fill(self) < 8) return 0;

    self->to_read -= 8;
    u64 ret = *self->next++ & UBYTE_MASK;
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
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
    atlib_compassert(self->next);

    if(self->to_read < 2 && __fill(self) < 2) return 0;
    self->to_read -= 2;

    i16 ret = *self->next++ & UBYTE_MASK;
    if(self->flags & BUFREAD_READ_BE) {
        return (ret << 8) | (*self->next++ & UBYTE_MASK);
    }
    else {
        return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    }
}

i16 atlib_bufread_read_i16_be(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);
    atlib_compassert(self->next);

    if(self->to_read < 2 && __fill(self) < 2) {
        return 0;
    }
    self->to_read -= 2;
    i16 ret = *self->next++ & UBYTE_MASK;
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

i16 atlib_bufread_read_i16_le(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);
    atlib_compassert(self->next);

    if(self->to_read < 2 && __fill(self) < 2) {
        return 0;
    }
    self->to_read -= 2;
    i16 ret = *self->next++ & UBYTE_MASK;
    return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
}

i32 atlib_bufread_read_i32(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);
    atlib_compassert(self->next);

    if(self->to_read < 4 && __fill(self) < 4) return 0;
    self->to_read -= 4;
    i32 ret = *self->next++ & UBYTE_MASK;
    
    if(self->flags & BUFREAD_READ_BE) {
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        return (ret << 8) | (*self->next++ & UBYTE_MASK);
    }
    else {
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 16);
        return (ret) | ((*self->next++ & UBYTE_MASK) << 24);
    }
}

i32 atlib_bufread_read_i32_be(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 4 && __fill(self) < 4) return 0;
    self->to_read -= 4;
    i32 ret = *self->next++ & UBYTE_MASK;
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

i32 atlib_bufread_read_i32_le(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 4 && __fill(self) < 4) return 0;
    self->to_read -= 4;
    i32 ret = *self->next++ & UBYTE_MASK;
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 16);
    return (ret) | ((*self->next++ & UBYTE_MASK) << 24);
}

i64 atlib_bufread_read_i64(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);
    atlib_compassert(self->next);

    if(self->to_read < 8 && __fill(self) < 8) return 0;
    self->to_read -= 8;
    i64 ret = *self->next++ & UBYTE_MASK;
    if(self->flags & BUFREAD_READ_BE) {
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
        return (ret << 8) | (*self->next++ & UBYTE_MASK);
    }
    else {
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
        return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    }
}

i64 atlib_bufread_read_i64_be(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 8 && __fill(self) < 8) return 0;

    self->to_read -= 8;
    i64 ret = *self->next++ & UBYTE_MASK;
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    ret = (ret << 8) | (*self->next++ & UBYTE_MASK);
    return (ret << 8) | (*self->next++ & UBYTE_MASK);
}

i64 atlib_bufread_read_i64_le(bufread_t * self) {
    atlib_compassert(self);
    atlib_compassert(self->fh);

    if(self->to_read < 8 && __fill(self) < 8) return 0;

    self->to_read -= 8;
    i64 ret = *self->next++ & UBYTE_MASK;
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    ret = (ret) | ((*self->next++ & UBYTE_MASK) << 8);
    return (ret) | ((*self->next++ & UBYTE_MASK) << 8);
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
