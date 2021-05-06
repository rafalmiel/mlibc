

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
			/* handle short options, i.e. options with only one dash prefixed; e.g. `program -s` */
			__ensure((strlen(argv[optind]) == 2) && "We do not support concatenated short options yet.");
			unsigned int i = 1;
			while(true) {
				auto opt = strchr(optstring, arg[i]);
				if(opt) {
					if(opt[1] == ':') {
						bool required = (opt[2] != ':');

						if(arg[i+1]) {
							optarg = arg + i + 1;
						} else if(optind + 1 < argc && argv[optind + 1] && (required || argv[optind + 1][0] != '-')) {
							optarg = argv[optind + 1];
							optind++;
						} else if(!required) {
							optarg = nullptr;
						} else {
							__ensure(!"We do not handle missing required short options yet.");
						}
					}

					optind++;
					return arg[i];
				} else {
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

