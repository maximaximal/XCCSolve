#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algorithm.h"
#include "log.h"
#include "parse.h"
#include "xcc.h"

static void
print_help() {
  printf("xccsolve -- solve XCC problems using different algorithms\n");
  printf("OPTIONS:\n");
  printf("  -h\t\tprint help\n");
  printf("  --file/-f\tset input file (also takes first positional arg)\n");
  printf("ALGORITHM SELECTORS:\n");
  printf("  --naive\tuse naive in-order for i selection\n");
  printf("  --mrv\t\tuse MRV for i selection (default)\n");
  printf("  -x\t\tuse Algorithm X\n");
  printf("  -c\t\tuse Algorithm C\n");
  printf("  -k\t\tuse CaDiCaL to solve with SAT (Knuth's trivial encoding)\n");
}

static void
parse_cli(xcc_config* cfg, int argc, char* argv[]) {
  int c;

  int sel[5];
  memset(sel, 0, sizeof(sel));

  struct option long_options[] = {
    { "verbose", no_argument, &cfg->verbose, 1 },
    { "file", required_argument, 0, 'f' },
    { "help", no_argument, 0, 'h' },
    { "naive", no_argument, &sel[0], XCC_ALGORITHM_NAIVE },
    { "mrv", no_argument, &sel[1], XCC_ALGORITHM_MRV },
    { "x", no_argument, &sel[2], XCC_ALGORITHM_X },
    { "c", no_argument, &sel[3], XCC_ALGORITHM_C },
    { "k", no_argument, &sel[4], XCC_ALGORITHM_KNUTH_CNF },
    { 0, 0, 0, 0 }
  };

  while(1) {

    int option_index = 0;

    c = getopt_long(argc, argv, "sxckhf:", long_options, &option_index);

    if(c == -1)
      break;

    switch(c) {
      case 'v':
        cfg->verbose = 1;
        break;
      case 'h':
        print_help();
        exit(EXIT_SUCCESS);
      case 'x':
        cfg->algorithm_select |= XCC_ALGORITHM_X;
        break;
      case 'c':
        cfg->algorithm_select |= XCC_ALGORITHM_C;
        break;
      case 'k':
        cfg->algorithm_select |= XCC_ALGORITHM_KNUTH_CNF;
        break;
      case 'f':
        cfg->input_file = optarg;
        break;
      default:
        break;
    }
  }

  if(optind < argc) {
    if(!cfg->input_file)
      cfg->input_file = argv[optind];
  }

  for(size_t i = 0; i < sizeof(sel) / sizeof(sel[0]); ++i)
    cfg->algorithm_select |= sel[i];
}

static int
process_file(xcc_config* cfg) {
  xcc_algorithm a;
  if(!xcc_algorithm_from_select(cfg->algorithm_select, &a)) {
    err("Could not extract algorithm from algorithm select! Try different "
        "algorithm selection.");
    return EXIT_FAILURE;
  }

  xcc_problem* p = xcc_parse_problem_file(&a, cfg->input_file);
  if(!p)
    return EXIT_FAILURE;

  int return_code = EXIT_SUCCESS;

  if(!a.compute_next_result) {
    err("Algorithm does not support solving!");
    return EXIT_FAILURE;
  }

  bool has_solution = a.compute_next_result(&a, p);
  if(!has_solution) {
    return_code = 20;
  } else {
    return_code = 10;

    xcc_link solution[p->l];
    xcc_extract_solution_option_indices(p, solution);
    for(size_t i = 0; i < p->l; ++i) {
      printf("%d ", solution[i]);
    }
    printf("\n");
  }

  xcc_problem_free(p, &a);
  return return_code;
}

int
main(int argc, char* argv[]) {
  xcc_config cfg;
  memset(&cfg, 0, sizeof(cfg));
  parse_cli(&cfg, argc, argv);

  if(cfg.input_file) {
    return process_file(&cfg);
  }

  return EXIT_SUCCESS;
}
