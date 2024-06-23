#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include "encryption.h"

static int cryptofs_getattr(const char *path, struct stat *stbuf) {
    int res;
    res = lstat(path, stbuf);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int cryptofs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(path);
    if (dp == NULL) {
        return -errno;
    }

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0)) {
            break;
        }
    }

    closedir(dp);
    return 0;
}

static int cryptofs_open(const char *path, struct fuse_file_info *fi) {
    int res;
    res = open(path, fi->flags);
    if (res == -1) {
        return -errno;
    }
    close(res);
    return 0;
}

static int cryptofs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;
    char *encrypted_buf;
    int output_len;

    (void) fi;
    fd = open(path, O_RDONLY);
    if (fd == -1) {
        return -errno;
    }

    encrypted_buf = malloc(size); // Allocate memory for the encrypted data
    if (!encrypted_buf) {
        close(fd);
        return -ENOMEM;
    }

    res = pread(fd, encrypted_buf, size, offset);
    if (res == -1) {
        free(encrypted_buf);
        close(fd);
        return -errno;
    } else {
        decrypt(encrypted_buf, buf, res, &output_len); // Decrypt the data
    }

    free(encrypted_buf);
    close(fd);
    return output_len;
}

static int cryptofs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;
    char *encrypted_buf;
    int output_len;

    (void) fi;
    fd = open(path, O_WRONLY);
    if (fd == -1) {
        return -errno;
    }

    encrypted_buf = malloc(size + EVP_MAX_BLOCK_LENGTH); // Allocate memory for the encrypted data
    if (!encrypted_buf) {
        close(fd);
        return -ENOMEM;
    }

    encrypt(buf, encrypted_buf, &output_len); // Encrypt the data
    res = pwrite(fd, encrypted_buf, output_len, offset);
    if (res == -1) {
        free(encrypted_buf);
        close(fd);
        return -errno;
    }

    free(encrypted_buf);
    close(fd);
    return res;
}

static int cryptofs_mknod(const char *path, mode_t mode, dev_t rdev) {
    int res;
    if (S_ISREG(mode)) {
        res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0) {
            res = close(res);
        }
    } else if (S_ISFIFO(mode)) {
        res = mkfifo(path, mode);
    } else {
        res = mknod(path, mode, rdev);
    }
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int cryptofs_mkdir(const char *path, mode_t mode) {
    int res;
    res = mkdir(path, mode);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int cryptofs_unlink(const char *path) {
    int res;
    res = unlink(path);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int cryptofs_rmdir(const char *path) {
    int res;
    res = rmdir(path);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int cryptofs_rename(const char *from, const char *to) {
    int res;
    res = rename(from, to);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int cryptofs_truncate(const char *path, off_t size) {
    int res;
    res = truncate(path, size);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static struct fuse_operations cryptofs_oper = {
    .getattr    = cryptofs_getattr,
    .readdir    = cryptofs_readdir,
    .open       = cryptofs_open,
    .read       = cryptofs_read,
    .write      = cryptofs_write,
    .mknod      = cryptofs_mknod,
    .mkdir      = cryptofs_mkdir,
    .unlink     = cryptofs_unlink,
    .rmdir      = cryptofs_rmdir,
    .rename     = cryptofs_rename,
    .truncate   = cryptofs_truncate,
    // Add other operations here
};

void init_keys_from_env() {
    char *key_env = getenv("CRYPTFS_KEY");
    char *iv_env = getenv("CRYPTFS_IV");

    if (key_env && iv_env) {
        set_key_iv(key_env, iv_env); // Set key and iv using encryption function
    } else {
        fprintf(stderr, "Error: CRYPTFS_KEY and CRYPTFS_IV must be set in the environment.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    init_openssl();       // Initialize OpenSSL
    init_keys_from_env(); // Initialize keys from environment

    return fuse_main(argc, argv, &cryptofs_oper, NULL);
}

