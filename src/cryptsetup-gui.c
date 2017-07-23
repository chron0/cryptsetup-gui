#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

bool decrypt(char* name, char* device, char* options, char* password);
bool mount(char* mountpoint);
char *strstrip(char *s);
void show_password_prompt(char* arg0, char* name);
void usage();

bool do_mount = false;
char* arg0 = NULL;
char *name = NULL, *device = NULL, *options = NULL;
char* mountpoint = "/dev/mapper/";

/*******************************************************************************
 * main
 * @param
 */

int main(int argc, char** argv) {

  #ifdef DEBUG
    printf("Starting cryptsetup-gui\n");
  #endif

  arg0 = *argv;
  argv++;
  argc--;

  if (argc == 0) {
    usage();
    exit(EXIT_SUCCESS);
  }

  if (strcmp(*argv, "-m") == 0) {
    do_mount = true;
    argv++;
    argc--;

    #ifdef DEBUG
      printf("Mount after unlocking\n");
    #endif

  }

  if (argc != 1) {
    usage();
    exit(EXIT_SUCCESS);
  }

  char* cryptpoint = *argv;
  char* ct = cryptpoint;

  #ifdef DEBUG
    printf("Verifying cryptpoint\n");
  #endif

  while (*ct != 0) {
    if (*ct < 'a' || *ct > 'z') {
      fprintf(stderr, "Non a-z cryptpoint given\n");
      exit(EXIT_FAILURE);
    }
    ct++;
  }

  #ifdef DEBUG
    printf("Cryptpoint verified\n");
  #endif

  size_t mps = strlen(mountpoint);
  size_t cps = strlen(cryptpoint);
  char* tmp = malloc(sizeof(char) * (mps + cps));
  strncpy(tmp, mountpoint, mps);
  strncpy(tmp + mps, cryptpoint, cps);
  mountpoint = tmp;

  #ifdef DEBUG
    printf("Mountpoint resolved to '%s'\n", mountpoint);
  #endif

  if (access(mountpoint, F_OK) == 0) {
    #ifdef DEBUG
      printf("/dev/mapper listing already exists\n");
      if (!do_mount)
        printf("Not told to mount automatically (-m), exiting...\n");
    #endif

    // Mountpoint already exists...
    if (do_mount && !mount(mountpoint)) {
      fprintf(stderr, "Failed to mount device %s\n", cryptpoint);
      exit(EXIT_FAILURE);
    }

    #ifdef DEBUG
      if (do_mount)
        printf("Mountpoint successfully mounted\n");
    #endif

    exit(EXIT_SUCCESS);
  }

  #ifdef DEBUG
    printf("Parsing crypttab\n");
  #endif

  FILE *f = fopen("/etc/crypttab", "re");
  if (!f) {
    perror("Failed to open crypttab for reading");
    exit(EXIT_FAILURE);
  }

  #ifdef DEBUG
    printf("Crypttab opened\n");
  #endif

  char *l, *p = NULL;
  char line[1024];
  int n = 0;

  for (;;) {
    int k;

    if (!fgets(line, sizeof(line), f))
      break;

    n++;

    l = strstrip(line);
    if (*l == '#' || *l == 0)
      continue;

    k = sscanf(l, "%ms %ms %ms %ms", &name, &device, &p, &options);
    free(p);
    p = NULL;
    if (k < 2 || k > 4) {
      fprintf(stderr, "Failed to parse /etc/crypttab:%u, ignoring\n", n);
      goto next;
    }

    if (strcmp(name, cryptpoint) == 0) {
      #ifdef DEBUG
        printf("Crypttab entry found for cryptpoint '%s'\n", device);
      #endif
      break;
    }

    next:
      free(name);
      free(device);
      free(options);
      name = NULL;
      device = NULL;
      options = NULL;
  }

  fclose(f);
  #ifdef DEBUG
    printf("Crypttab parsing finished\n");
  #endif

  if (name == NULL || device == NULL) {
    fprintf(stderr, "Entry for %s not found in crypttab\n", cryptpoint);
  }

  show_password_prompt(arg0, name);

  return 0;
}

/*******************************************************************************
 * unlock
 * @param
 */

bool unlock(char* password) {
  // Try decrypting (note that password is not needed)
  if (!decrypt(name, device, options, password)) {
    fprintf(stderr, "Failed to decrypt device %s\n", name);
    return false;
  }
  if (do_mount) {
    if (!mount(mountpoint)) {
      fprintf(stderr, "Failed to mount device %s\n", name);
      return false;
    }
  }
  return true;
}

/*******************************************************************************
 * decrypt
 * @param
 */

bool decrypt(char* name, char* device, char* options, char* password) {
  // TODO: Respect options list
  // We need to be weary of a bug in cryptsetup
  // https://groups.google.com/forum/#!msg/linux.debian.bugs.dist/7yRXc5NGMJM/q80hakUzDVMJ
  // cryptsetup drops privileges if EUID != UID
  // so, we store the old UID so we can restore it later
  uid_t ruid = getuid();

  char* flags = "";
  if (strstr(options, "allow-discards") != NULL) {
    flags = "--allow-discards";
  }

  char* command = NULL;
  int cres = asprintf(&command, "/sbin/cryptsetup %s -q luksOpen %s %s", flags, device, name);
  if(cres == -1) {
    #ifdef DEBUG
      printf("ERROR: cryptsetup luksopen\n");
    #endif
    exit(EXIT_FAILURE);
  }

  int sres = setreuid(0, 0);
  if(sres == -1) {
    #ifdef DEBUG
      printf("ERROR: setreuid\n");
    #endif
    exit(EXIT_FAILURE);
  }

  fflush(stdout);
  FILE *crypt = popen(command, "w");
  free(command);
  fprintf(crypt, "%s\n%d", password, EOF);
  int ret = pclose(crypt);

  // restore UID
  sres = setreuid(ruid, 0);
  if(sres == -1) {
    #ifdef DEBUG
      printf("ERROR: setreuid\n");
    #endif
    exit(EXIT_FAILURE);
  }

  return WEXITSTATUS(ret) == 0;
}

/*******************************************************************************
 * mount
 * @param
 */

bool mount(char* mountpoint) {
  // Apparently, we need this EUID != UID fix for mount as well...
  uid_t ruid = getuid();
  char* command = NULL;

  fflush(stdout);
  int mres = asprintf(&command, "/bin/mount %s", mountpoint);
  if(mres == -1) {
    #ifdef DEBUG
      printf("ERROR: Mount failed\n");
    #endif
    exit(EXIT_FAILURE);
  }

  int sres = setreuid(0, 0);
  if(sres == -1) {
    #ifdef DEBUG
      printf("ERROR: setreuid\n");
    #endif
    exit(EXIT_FAILURE);
  }

  FILE *mnt = popen(command, "r");
  int ret = pclose(mnt);

  sres = setreuid(ruid, 0);
  if(sres == -1) {
    #ifdef DEBUG
      printf("ERROR: setreuid\n");
    #endif
    exit(EXIT_FAILURE);
  }

  return WEXITSTATUS(ret) == 0;
}

/*******************************************************************************
 * show_password_prompt
 * @param
 */

void show_password_prompt(char* arg0, char* name) {

  char password[1024];

  #ifdef DEBUG
    printf("Showing GTK GUI\n");
    printf("Dropping permissions\n");
  #endif

  // We don't want to give the GTK any root access
  uid_t ruid = getuid();
  int sres = seteuid(ruid);
  if(sres == -1) {
    #ifdef DEBUG
      printf("ERROR: seteuid\n");
    #endif
    exit(EXIT_FAILURE);
  }

  #ifdef DEBUG
    printf("Permissions dropped\n");
  #endif

  fflush(stdout);

  char* command = NULL;
  int gtkres = asprintf(&command, "%s-gtk %s", arg0, name);
  if(gtkres == -1) {
    #ifdef DEBUG
      printf("ERROR: cryptsetup-gui-gtk call failed\n");
    #endif
    exit(EXIT_FAILURE);
  }

  FILE *pw = popen(command, "r");
  free(command);

  #ifdef DEBUG
    printf("GTK GUI started\n");
  #endif

  // Now get the password


  if (fgets(password, 1024, pw) == NULL) {
    printf("Password not received\n");
    exit(EXIT_FAILURE);
  }

  strtok(password, "\n");

  #ifdef DEBUG
    printf("Password received\n");
  #endif

  pclose(pw);
  fflush(stdout);

  #ifdef DEBUG
    printf("GTK GUI closed, resuming root\n");
  #endif

  // Need root to do the unlocking
  sres = seteuid(0);
  if(sres == -1) {
    #ifdef DEBUG
      printf("ERROR: seteuid\n");
    #endif
    exit(EXIT_FAILURE);
  }

  #ifdef DEBUG
    printf("EUID now %d, unlocking...\n", geteuid());
  #endif

  if (unlock(password)) {
    #ifdef DEBUG
      printf("Unlocked successfully\n");
    #endif
  } else {
    fflush(stdout);
    fprintf(stderr, "Invalid password given, unlock not possible\n");
    exit(EXIT_FAILURE);
  }
}

/*******************************************************************************
 * is_space - Thank your kernel
 * @param
 */

bool is_space(char s) {
  return s == ' ' || s == '\t' || s == '\n';
}

/*******************************************************************************
 * strstrip
 * @param
 */

char *strstrip(char *s) {
  size_t size;
  char *end;

  size = strlen(s);

  if (!size)
    return s;

  end = s + size - 1;

  while (end >= s && is_space(*end))
    end--;
  *(end + 1) = '\0';

  while (*s && is_space(*s))
    s++;

  return s;
}

/*******************************************************************************
 * usage
 */

void usage() {
  printf("Usage: %s [-m] cryptpoint\n", arg0);
}
