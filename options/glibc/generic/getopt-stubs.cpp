/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define NO_ARG          	no_argument
#define REQUIRED_ARG    	required_argument
#define OPTIONAL_ARG    	optional_argument

typedef struct getopt_data
{
  char *optarg;
  int optind, opterr, optopt, optwhere;
} getopt_data;

/* macros */

/* types */
typedef enum GETOPT_ORDERING_T
{
  PERMUTE,
  RETURN_IN_ORDER,
  REQUIRE_ORDER
} GETOPT_ORDERING_T;

/* globally-defined variables */
char *optarg = 0;
int optind = 0;
int opterr = 1;
int optopt = '?';

/* static variables */
static int optwhere = 0;

/* functions */

/* reverse_argv_elements:  reverses num elements starting at argv */
static void
reverse_argv_elements (char **argv, int num)
{
  int i;
  char *tmp;

  for (i = 0; i < (num >> 1); i++)
    {
      tmp = argv[i];
      argv[i] = argv[num - i - 1];
      argv[num - i - 1] = tmp;
    }
}

/* permute: swap two blocks of argv-elements given their lengths */
static void
permute (char *const argv[], int len1, int len2)
{
  reverse_argv_elements ((char **) argv, len1);
  reverse_argv_elements ((char **) argv, len1 + len2);
  reverse_argv_elements ((char **) argv, len2);
}

/* is_option: is this argv-element an option or the end of the option list? */
static int
is_option (char *argv_element, int only)
{
  return ((argv_element == 0)
	  || (argv_element[0] == '-') || (only && argv_element[0] == '+'));
}

/* read_globals: read the values from the globals into a getopt_data 
   structure */
static void
read_globals (struct getopt_data *data)
{
  data->optarg = optarg;
  data->optind = optind;
  data->opterr = opterr;
  data->optopt = optopt;
  data->optwhere = optwhere;
}

/* write_globals: write the values into the globals from a getopt_data
   structure */
static void
write_globals (struct getopt_data *data)
{
  optarg = data->optarg;
  optind = data->optind;
  opterr = data->opterr;
  optopt = data->optopt;
  optwhere = data->optwhere;
}

/* getopt_internal:  the function that does all the dirty work */
static int
getopt_internal (int argc, char *const argv[], const char *shortopts,
		 const struct option *longopts, int *longind, int only,
		 struct getopt_data *data)
{
  GETOPT_ORDERING_T ordering = PERMUTE;
  size_t permute_from = 0;
  int num_nonopts = 0;
  int optindex = 0;
  size_t match_chars = 0;
  char *possible_arg = 0;
  int longopt_match = -1;
  int has_arg = -1;
  char *cp = 0;
  int arg_next = 0;
  int initial_colon = 0;

  /* first, deal with silly parameters and easy stuff */
  if (argc == 0 || argv == 0 || (shortopts == 0 && longopts == 0)
      || data->optind >= argc || argv[data->optind] == 0)
    return EOF;
  if (strcmp (argv[data->optind], "--") == 0)
    {
      data->optind++;
      return EOF;
    }

  /* if this is our first time through */
  if (data->optind == 0)
    data->optind = data->optwhere = 1;

  /* define ordering */
  if (shortopts != 0 && (*shortopts == '-' || *shortopts == '+'))
    {
      ordering = (*shortopts == '-') ? RETURN_IN_ORDER : REQUIRE_ORDER;
      shortopts++;
    }
  else
    ordering = (getenv ("POSIXLY_CORRECT") != 0) ? REQUIRE_ORDER : PERMUTE;

  /* check for initial colon in shortopts */
  if (shortopts != 0 && *shortopts == ':')
    {
      ++shortopts;
      initial_colon = 1;
    }

  /*
   * based on ordering, find our next option, if we're at the beginning of
   * one
   */
  if (data->optwhere == 1)
    {
      switch (ordering)
	{
	default:		/* shouldn't happen */
	case PERMUTE:
	  permute_from = data->optind;
	  num_nonopts = 0;
	  while (!is_option (argv[data->optind], only))
	    {
	      data->optind++;
	      num_nonopts++;
	    }
	  if (argv[data->optind] == 0)
	    {
	      /* no more options */
	      data->optind = permute_from;
	      return EOF;
	    }
	  else if (strcmp (argv[data->optind], "--") == 0)
	    {
	      /* no more options, but have to get `--' out of the way */
	      permute (argv + permute_from, num_nonopts, 1);
	      data->optind = permute_from + 1;
	      return EOF;
	    }
	  break;
	case RETURN_IN_ORDER:
	  if (!is_option (argv[data->optind], only))
	    {
	      data->optarg = argv[data->optind++];
	      return (data->optopt = 1);
	    }
	  break;
	case REQUIRE_ORDER:
	  if (!is_option (argv[data->optind], only))
	    return EOF;
	  break;
	}
    }
  /* we've got an option, so parse it */

  /* first, is it a long option? */
  if (longopts != 0
      && (memcmp (argv[data->optind], "--", 2) == 0
	  || (only && argv[data->optind][0] == '+')) && data->optwhere == 1)
    {
      /* handle long options */
      if (memcmp (argv[data->optind], "--", 2) == 0)
	data->optwhere = 2;
      longopt_match = -1;
      possible_arg = strchr (argv[data->optind] + data->optwhere, '=');
      if (possible_arg == 0)
	{
	  /* no =, so next argv might be arg */
	  match_chars = strlen (argv[data->optind]);
	  possible_arg = argv[data->optind] + match_chars;
	  match_chars = match_chars - data->optwhere;
	}
      else
	match_chars = (possible_arg - argv[data->optind]) - data->optwhere;
      for (optindex = 0; longopts[optindex].name != 0; ++optindex)
	{
	  if (memcmp
	      (argv[data->optind] + data->optwhere, longopts[optindex].name,
	       match_chars) == 0)
	    {
	      /* do we have an exact match? */
	      if (match_chars == (int) (strlen (longopts[optindex].name)))
		{
		  longopt_match = optindex;
		  break;
		}
	      /* do any characters match? */
	      else
		{
		  if (longopt_match < 0)
		    longopt_match = optindex;
		  else
		    {
		      /* we have ambiguous options */
		      if (data->opterr)
			fprintf (stderr, "%s: option `%s' is ambiguous "
				 "(could be `--%s' or `--%s')\n",
				 argv[0],
				 argv[data->optind],
				 longopts[longopt_match].name,
				 longopts[optindex].name);
		      return (data->optopt = '?');
		    }
		}
	    }
	}
      if (longopt_match >= 0)
	has_arg = longopts[longopt_match].has_arg;
    }

  /* if we didn't find a long option, is it a short option? */
  if (longopt_match < 0 && shortopts != 0)
    {
      cp = strchr (shortopts, argv[data->optind][data->optwhere]);
      if (cp == 0)
	{
	  /* couldn't find option in shortopts */
	  if (data->opterr)
	    fprintf (stderr,
		     "%s: invalid option -- `-%c'\n",
		     argv[0], argv[data->optind][data->optwhere]);
	  data->optwhere++;
	  if (argv[data->optind][data->optwhere] == '\0')
	    {
	      data->optind++;
	      data->optwhere = 1;
	    }
	  return (data->optopt = '?');
	}
      has_arg = ((cp[1] == ':')
		 ? ((cp[2] == ':') ? OPTIONAL_ARG : REQUIRED_ARG) : NO_ARG);
      possible_arg = argv[data->optind] + data->optwhere + 1;
      data->optopt = *cp;
    }

  /* get argument and reset data->optwhere */
  arg_next = 0;
  switch (has_arg)
    {
    case OPTIONAL_ARG:
      if (*possible_arg == '=')
	possible_arg++;
      data->optarg = (*possible_arg != '\0') ? possible_arg : 0;
      data->optwhere = 1;
      break;
    case REQUIRED_ARG:
      if (*possible_arg == '=')
	possible_arg++;
      if (*possible_arg != '\0')
	{
	  data->optarg = possible_arg;
	  data->optwhere = 1;
	}
      else if (data->optind + 1 >= argc)
	{
	  if (data->opterr)
	    {
	      fprintf (stderr, "%s: argument required for option `", argv[0]);
	      if (longopt_match >= 0)
		{
		  fprintf (stderr, "--%s'\n", longopts[longopt_match].name);
		  data->optopt = initial_colon ? ':' : '\?';
		}
	      else
		{
		  fprintf (stderr, "-%c'\n", *cp);
		  data->optopt = *cp;
		}
	    }
	  data->optind++;
	  return initial_colon ? ':' : '\?';
	}
      else
	{
	  data->optarg = argv[data->optind + 1];
	  arg_next = 1;
	  data->optwhere = 1;
	}
      break;
    default:			/* shouldn't happen */
    case NO_ARG:
      if (longopt_match < 0)
	{
	  data->optwhere++;
	  if (argv[data->optind][data->optwhere] == '\0')
	    data->optwhere = 1;
	}
      else
	data->optwhere = 1;
      data->optarg = 0;
      break;
    }

  /* do we have to permute or otherwise modify data->optind? */
  if (ordering == PERMUTE && data->optwhere == 1 && num_nonopts != 0)
    {
      permute (argv + permute_from, num_nonopts, 1 + arg_next);
      data->optind = permute_from + 1 + arg_next;
    }
  else if (data->optwhere == 1)
    data->optind = data->optind + 1 + arg_next;

  /* finally return */
  if (longopt_match >= 0)
    {
      if (longind != 0)
	*longind = longopt_match;
      if (longopts[longopt_match].flag != 0)
	{
	  *(longopts[longopt_match].flag) = longopts[longopt_match].val;
	  return 0;
	}
      else
	return longopts[longopt_match].val;
    }
  else
    return data->optopt;
}

int
getopt (int argc, char *const argv[], const char *optstring)
{
  struct getopt_data data;
  int r;

  read_globals (&data);
  r = getopt_internal (argc, argv, optstring, 0, 0, 0, &data);
  write_globals (&data);
  return r;
}

int
getopt_long (int argc, char *const argv[], const char *shortopts,
	     const struct option *longopts, int *longind)
{
  struct getopt_data data;
  int r;

  read_globals (&data);
  r = getopt_internal (argc, argv, shortopts, longopts, longind, 0, &data);
  write_globals (&data);
  return r;
}

int
getopt_long_only (int argc, char *const argv[], const char *shortopts,
		  const struct option *longopts, int *longind)
{
  struct getopt_data data;
  int r;

  read_globals (&data);
  r = getopt_internal (argc, argv, shortopts, longopts, longind, 1, &data);
  write_globals (&data);
  return r;
}

int
__getopt_r (int argc, char *const argv[], const char *optstring,
	    struct getopt_data *data)
{
  return getopt_internal (argc, argv, optstring, 0, 0, 0, data);
}

int
__getopt_long_r (int argc, char *const argv[], const char *shortopts,
		 const struct option *longopts, int *longind,
		 struct getopt_data *data)
{
  return getopt_internal (argc, argv, shortopts, longopts, longind, 0, data);
}

int
__getopt_long_only_r (int argc, char *const argv[], const char *shortopts,
		      const struct option *longopts, int *longind,
		      struct getopt_data *data)
{
  return getopt_internal (argc, argv, shortopts, longopts, longind, 1, data);
}

#if 0
#include <assert.h>
#include <bits/ensure.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include <mlibc/debug.hpp>
#include <stdlib.h>
#include <limits.h>

char *optarg;
int optind = 1;
int opterr = 1;
int optopt;
int optpos;

#if 0
int getopt_long(int argc, char * const argv[], const char *optstring,
		const struct option *longopts, int *longindex) {
	// glibc extension: Setting optind to zero causes a full reset.
	// TODO: Should we really reset opterr and the other flags?
	if(!optind) {
		optarg = nullptr;
		optind = 1;
		opterr = 1;
		optopt = 0;
	}

	while(optind < argc) {
		char *arg = argv[optind];
		if(arg[0] != '-')
			return -1;

		if(arg[1] == '-') {
			arg += 2;

			// Determine the end of the option name (vs. the start of the argument).
			auto s = strchr(arg, '=');
			size_t n = s ? (s - arg) : strlen(arg);

			int k = -1;
			for(int i = 0; longopts[i].name; i++) {
				if(strncmp(arg, longopts[i].name, n) || longopts[i].name[n])
					continue;

				if(k >= 0) {
					if(opterr)
						fprintf(stderr, "Multiple option declaration detected.\n", arg);
					return '?';
				}
				k = i;
			}

			if(k == -1) {
				if(opterr)
					fprintf(stderr, "--%s is not a valid option.\n", arg);
				return '?';
			}

			if(longindex)
				*longindex = k;

			// Consume the option and its argument.
			if(longopts[k].has_arg == required_argument) {
				if(s) {
					// Consume the long option and its argument.
					optarg = s + 1;
					optind++;
				}else if(argv[optind + 1]) {
					// Consume the long option.
					optind++;

					// Consume the option's argument.
					optarg = argv[optind];
					optind++;
				}else{
					if(opterr)
						fprintf(stderr, "--%s requires an argument.\n", arg);
					return '?';
				}
			}else if(longopts[k].has_arg == optional_argument) {
				if(s) {
					// Consume the long option and its argument.
					optarg = s + 1;
					optind++;
				}else{
					// Consume the long option.
					optarg = nullptr;
					optind++;
				}
			}else{
				__ensure(longopts[k].has_arg == no_argument);

				// Consume the long option.
				optind++;
			}

			if(!longopts[k].flag) {
				return longopts[k].val;
			}else{
				*longopts[k].flag = longopts[k].val;
				return 0;
			}
		}else{
			__ensure((strlen(argv[optind]) == 2) && "We do not support concatenated short options yet.");
			unsigned int i = 1;
			while(true) {
				auto opt = strchr(optstring, arg[i]);
				if(opt) {
					__ensure((opt[1] != ':') && "We do not support option arguments.");
					optind++;
					return arg[i];
				}else {
					optopt = arg[1];
					if(opterr)
						fprintf(stderr, "%s is not a valid option.\n", arg);
					return '?';
				}
			}
		}
	}
	return -1;
}

int getopt_long_only(int, char *const[], const char *, const struct option *, int *) {
	mlibc::infoLogger() << "\e[31mmlibc: getopt_long_only() is only a stub\e[39m" << frg::endlog;
	return -1;
}
#endif

static void permute(char *const *argv, int dest, int src)
{
	char **av = (char **)argv;
	char *tmp = av[src];
	int i;
	for (i=src; i>dest; i--)
		av[i] = av[i-1];
	av[dest] = tmp;
}

static int __getopt_long_core(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx, int longonly);

static int __getopt_long(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx, int longonly)
{
	int ret, skipped, resumed;
	if (!optind) {
		optpos = 0;
		optind = 1;
	}
	if (optind >= argc || !argv[optind]) return -1;
	skipped = optind;
	if (optstring[0] != '+' && optstring[0] != '-') {
		int i;
		for (i=optind; ; i++) {
			if (i >= argc || !argv[i]) return -1;
			if (argv[i][0] == '-' && argv[i][1]) break;
		}
		optind = i;
	}
	resumed = optind;
	ret = __getopt_long_core(argc, argv, optstring, longopts, idx, longonly);
	if (resumed > skipped) {
		int i, cnt = optind-resumed;
		for (i=0; i<cnt; i++)
			permute(argv, skipped, optind-1);
		optind = skipped + cnt;
	}
	return ret;
}

static int __getopt_long_core(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx, int longonly)
{
	optarg = 0;
	if (longopts && argv[optind][0] == '-' &&
		((longonly && argv[optind][1] && argv[optind][1] != '-') ||
		 (argv[optind][1] == '-' && argv[optind][2])))
	{
		int colon = optstring[optstring[0]=='+'||optstring[0]=='-']==':';
		int i, cnt, match;
		char *arg, *opt, *start = argv[optind]+1;
		for (cnt=i=0; longopts[i].name; i++) {
			const char *name = longopts[i].name;
			opt = start;
			if (*opt == '-') opt++;
			while (*opt && *opt != '=' && *opt == *name)
				name++, opt++;
			if (*opt && *opt != '=') continue;
			arg = opt;
			match = i;
			if (!*name) {
				cnt = 1;
				break;
			}
			cnt++;
		}
		if (cnt==1 && longonly && arg-start == mblen(start, MB_LEN_MAX)) {
			int l = arg-start;
			for (i=0; optstring[i]; i++) {
				int j;
				for (j=0; j<l && start[j]==optstring[i+j]; j++);
				if (j==l) {
					cnt++;
					break;
				}
			}
		}
		if (cnt==1) {
			i = match;
			opt = arg;
			optind++;
			if (*opt == '=') {
				if (!longopts[i].has_arg) {
					optopt = longopts[i].val;
					if (colon || !opterr)
						return '?';
							fprintf(stderr,
								"%s: option does not take an argument: %s %d",
								argv[0], longopts[i].name,
								strlen(longopts[i].name));
					return '?';
				}
				optarg = opt+1;
			} else if (longopts[i].has_arg == required_argument) {
				if (!(optarg = argv[optind])) {
					optopt = longopts[i].val;
					if (colon) return ':';
					if (!opterr) return '?';
						fprintf(stderr,
							"%s: option requires an argument: %s %d",
							argv[0], longopts[i].name,
							strlen(longopts[i].name));
					return '?';
				}
				optind++;
			}
			if (idx) *idx = i;
			if (longopts[i].flag) {
				*longopts[i].flag = longopts[i].val;
				return 0;
			}
			return longopts[i].val;
		}
		if (argv[optind][1] == '-') {
			optopt = 0;
			if (!colon && opterr)
				fprintf(stderr, cnt ?
					"%s: option is ambiguous: %s %d" :
					"%s: unrecognized option: %s %d",
					argv[0], argv[optind]+2,
					strlen(argv[optind]+2));
			optind++;
			return '?';
		}
	}
	return getopt(argc, argv, optstring);
}

int getopt_long(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx)
{
	return __getopt_long(argc, argv, optstring, longopts, idx, 0);
}

int getopt_long_only(int argc, char *const *argv, const char *optstring, const struct option *longopts, int *idx)
{
	return __getopt_long(argc, argv, optstring, longopts, idx, 1);
}
#endif
