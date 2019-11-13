#include "common.h"

static bool confirm_dist(const int v, const int w, const int height, const int r)
{
  int w0 = WIDTH (v, height);
  int h0 = HEIGHT(v, height);
  int w1 = WIDTH (w, height);
  int h1 = HEIGHT(w, height);
  int distance = abs(w0 - w1) + abs(h0 - h1);
  return (distance <= r);
}

static void two_toggle_operation(const int height, const int d, const int r,
				 const int E, int list_E[])
{
  int e1, e2, new_e1_v, new_e1_w, new_e2_v, new_e2_w;
  unsigned int round = 0;

  while(round < 2*E){
    do{
      e1 = random() % E;
      e2 = random() % E;
    } while( e1 == e2 );
    int e1_v = list_E[2*e1]; int e1_w = list_E[2*e1+1];
    int e2_v = list_E[2*e2]; int e2_w = list_E[2*e2+1];
    if(confirm_dist(e1_v, e2_v, height, r) && confirm_dist(e1_w, e2_w, height, r)){
      new_e1_v = e1_v;  new_e1_w = e2_v;
      new_e2_v = e1_w;  new_e2_w = e2_w;
    }
    else if(confirm_dist(e1_v, e2_w, height, r) && confirm_dist(e1_w, e2_v, height, r)){
      new_e1_v = e1_v;  new_e1_w = e2_w;
      new_e2_v = e1_w;  new_e2_w = e2_v;
    }
    else{
      continue;
    }
    list_E[2*e1] = new_e1_v;  list_E[2*e1+1] = new_e1_w;
    list_E[2*e2] = new_e2_v;  list_E[2*e2+1] = new_e2_w;
    round++;
  }
}

static void create_lattice(const int lines, int edge[lines*2], const int width, const int height,
			   const int degree, const int low_length, const int random_seed,
			   const long long ncalcs)
{
  // Inherited from http://research.nii.ac.jp/graphgolf/c/create-lattice.c
  int nodes = width * height, eid = 0;
  for(int x=0;x<width/2;x++){
    for(int y=0;y<height;y++){
      for(int k=0;k<degree;k++){
	edge[2*eid]   = y + 2 * x * height;
	edge[2*eid+1] = edge[2*eid] + height;
        eid++;
      }
    }
  }

  if(width%2 == 1){
    for(int y=0;y<height/2;y++){
      for(int k=0;k<degree;k++){
	edge[2*eid]   = (width - 1) * height + 2 * y;
	edge[2*eid+1] = edge[2*eid] + 1;
        eid++;
      }
    }

    /* add self-loop */
    if(height%2 == 1){
      for(int k=0;k<degree/2;k++){
	edge[2*eid] = edge[2*eid+1] = nodes - 1;
	eid++;
      }
    }
  }

  int (*adjacency)[degree] = malloc(sizeof(int)*nodes*degree); // int adjacency[nodes][degree];
  int *rotate_hash = malloc(nodes * sizeof(int));
  int diam;
  double ASPL;
  long long num = 0;
  create_rotate_hash(nodes, height, width, 1, rotate_hash);
  while(1){
    two_toggle_operation(height, degree, low_length, lines, edge);
    create_adjacency(nodes, lines, degree, (const int (*)[2])edge, adjacency);
    if(evaluation(nodes, degree, 1, (const int* restrict)adjacency,
      		  nodes, height, height, &diam, &ASPL, true, rotate_hash))
      break;
    else if(num > ncalcs/(2*lines))
      ERROR("Cannot create initial file\n");
    else
      num++;
  }
  
  free(adjacency);
  free(rotate_hash);
  /*
    for(int i=0;i<lines;i++)
      printf("%d,%d %d,%d\n",
    	   WIDTH (edge[i*2],   height),
    	   HEIGHT(edge[i*2],   height),
    	   WIDTH (edge[i*2+1], height),
    	   HEIGHT(edge[i*2+1], height));*/
  //  EXIT(0);
}

static void print_help(char *argv)
{
  END("%s [-f <edge_file>] [-r length] [-o <output_file>] [-s <random_seed>] [-n <calculations>] [-w <max_temperature>]\
 [-c <min_temperature>] [-C <cooling_cycle>] [-B] [-d] [-F] [-Y] [-M] [-N] [-h] [-W] [-H] [-D]\n", argv);
}

static void set_args(const int argc, char **argv, char *infname, int *low_length, char *outfname, bool *enable_outfname,
		     int *random_seed, long long *ncalcs, double *max_temp, bool *enable_max_temp, double *min_temp,
		     bool *enable_min_temp, int *cooling_cycle, bool *enable_hill_climbing,
		     bool *enable_detect_temp, bool *enable_bfs, bool *enable_halfway, double *fixed_temp,
		     bool *enable_fixed_temp, bool *enable_infname, int *width, int *height, int *degree)
{
  if(argc < 3)
    print_help(argv[0]);

  int result;
  while((result = getopt(argc,argv,"f:o:r:s:n:w:c:C:BdF:YMhW:H:D:"))!=-1){
    switch(result){
    case 'W':
      *width = atoi(optarg);
      if(*width <= 0)
	ERROR("-W value > 0\n");
      break;
    case 'H':
      *height = atoi(optarg);
      if(*height <= 0)
	 ERROR("-H value > 0\n");
      break;
    case 'D':
      *degree = atoi(optarg);
      if(*degree <= 0)
	ERROR("-D value > 0\n");
      break;
    case 'f':
      if(strlen(optarg) > MAX_FILENAME_LENGTH)
        ERROR("Input filename is long (%s). Please change MAX_FILENAME_LENGTH.\n", optarg);
      strcpy(infname, optarg);
      *enable_infname = true;
      break;
    case 'o':
      if(strlen(optarg) > MAX_FILENAME_LENGTH)
        ERROR("Output filename is long (%s). Please change MAX_FILENAME_LENGTH.\n", optarg);
      strcpy(outfname, optarg);
      *enable_outfname = true;
      break;
     case 'r':
      *low_length = atoi(optarg);
      if(*low_length <= 0)
        ERROR("-r value > 0\n");
      break;
    case 's':
      *random_seed = atoi(optarg);
      if(*random_seed < 0)
        ERROR("-s value >= 0\n");
      break;
    case 'n':
      *ncalcs = atoll(optarg);
      if(*ncalcs < 0)
        ERROR("-n value >= 0\n");
      break;
    case 'w':
      *max_temp = atof(optarg);
      if(*max_temp <= 0)
        ERROR("-w value > 0\n");
      *enable_max_temp = true;
      break;
    case 'c':
      *min_temp = atof(optarg);
      if(*min_temp <= 0)
        ERROR("MIN value > 0\n");
      *enable_min_temp = true;
      break;
    case 'C':
      *cooling_cycle = atoi(optarg);
      if(*cooling_cycle <= 0)
	ERROR("Cooling Cycle > 0\n");
      break;
    case 'B':
      *enable_bfs = true;
      break;
    case 'd':
      *enable_detect_temp = true;
      break;
    case 'F':
      *fixed_temp = atof(optarg);
      if(*fixed_temp <= 0)
	ERROR("-F value > 0\n");
      *enable_fixed_temp = true;
      break;
    case 'Y':
      *enable_hill_climbing = true;
      break;
    case 'M':
      *enable_halfway = true;
      break;
    case 'h':
    default:
      print_help(argv[0]);
    }
  }
}

static int count_lines(const char *fname)
{
  FILE *fp = NULL;
  if((fp = fopen(fname, "r")) == NULL)
    ERROR("File not found\n");
  
  int lines = 0, c;
  while((c = fgetc(fp)) != EOF)
    if(c == '\n')
      lines++;

  fclose(fp);
  return lines;
}

static void read_file_lattice(int (*edge)[2], int *w, int *h, const char *fname)
{
  FILE *fp;
  if((fp = fopen(fname, "r")) == NULL){
    PRINT_R0("File not found\n");
    EXIT(1);
  }

  int n[4];
  *w = 0;
  *h = 0;
  while(fscanf(fp, "%d,%d %d,%d", &n[0], &n[1], &n[2], &n[3]) != EOF){
    *w = MAX(*w, n[0]);
    *h = MAX(*h, n[1]);
    *w = MAX(*w, n[2]);
    *h = MAX(*h, n[3]);
  }
  *w += 1;
  *h += 1;
  rewind(fp);

  int i = 0;
  while(fscanf(fp, "%d,%d %d,%d", &n[0], &n[1], &n[2], &n[3]) != EOF){
    edge[i][0] = n[0] * (*h) + n[1];
    edge[i][1] = n[2] * (*h) + n[3];
    i++;
  }

  fclose(fp);
}

static int max_node_num(const int lines, const int edge[lines*2])
{
  int max = edge[0];
  for(int i=1;i<lines*2;i++)
    max = MAX(max, edge[i]);

  return max;
}

static int dist(const int x1, const int y1, const int x2, const int y2)
{
  return(abs(x1 - x2) + abs(y1 - y2));
}

// This function is inherited from "http://research.nii.ac.jp/graphgolf/pl/lower-lattice.pl".
static void lower_bound_of_diam_aspl(int *low_diam, double *low_ASPL, const int m, const int n,
				     const int degree, const int length)
{
  int mn = m * n;
  int maxhop = MAX((m+n-2)/length,log(mn/degree)/log(degree-1)-1)+2;
  double sum = 0, current = degree;
  double moore[maxhop+1], hist[maxhop+1], mh[maxhop+1];

  for(int i=0;i<=maxhop;i++)
    moore[i] = hist[i] = 0;

  moore[0] = 1;
  moore[1] = degree + 1;
  for(int i=2;i<=maxhop;i++){
    current = current * (degree - 1);
    moore[i] = moore[i-1] + current;
    if(moore[i] > mn)
      moore[i] = mn;
  }

  for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
      for(int k=0;k<=maxhop;k++)
        hist[k]=0;

      for (int i2=0;i2<m;i2++)
        for(int j2=0;j2<n;j2++)
          hist[(dist(i,j,i2,j2)+length-1)/length]++;

      for(int k=1;k<=maxhop;k++)
        hist[k] += hist[k-1];

      for(int k=0;k<=maxhop;k++)
        mh[k] = MIN(hist[k], moore[k]);

      for(int k=1;k<=maxhop;k++){
        sum += (double)(mh[k] - mh[k-1]) * k;
      }
    }
  }

  int dboth = 0;
  for(dboth=0;;dboth++)
    if(mh[dboth] == mn)
      break;

  *low_diam = dboth;
  *low_ASPL = sum/((double)mn*(mn-1));
}

static void output_params(const int degree, const int low_length, const int random_seed,
			  const double max_temp, const double min_temp, const long long ncalcs,
			  const int cooling_cycle, const double cooling_rate, const char *infname,
			  const char *outfname, const bool enable_outfname, const double average_time,
			  const bool enable_hill_climbing, const int width, const int height, const bool enable_bfs,
			  const bool enable_fixed_temp, const double fixed_temp)
			  
{
#ifdef NDEBUG
  PRINT_R0("NO DEBUG MODE\n");
#else
  PRINT_R0("DEBUG MODE\n");
#endif
  PRINT_R0("Seed     : %d\n", random_seed);
  PRINT_R0("Processes: %d\n", procs);
#ifdef _OPENMP
  PRINT_R0("Threads  : %d\n", omp_get_max_threads());
#endif
  if(enable_bfs) PRINT_R0("APSP     : BFS\n");
  else           PRINT_R0("APSP     : MATRIX Opetation\n");

  if(enable_hill_climbing == false){
    if(enable_fixed_temp)
      PRINT_R0("Algorithm: Fixed Temperature Simulated Annealing (2-opt) : %f\n", fixed_temp);
    else
      PRINT_R0("Algorithm: Simulated Annealing (2-opt)\n");

    PRINT_R0("   MAX Temperature: %f\n", max_temp);
    PRINT_R0("   MIN Temperature: %f\n", min_temp);
    PRINT_R0("   Cooling Cycle: %d\n", cooling_cycle);
    PRINT_R0("   Cooling Rate : %f\n", cooling_rate);
  }
  else{
    PRINT_R0("Algorithm: Hill climbing Method (2-opt)\n");
  }

  PRINT_R0("Num. of Calulations: %lld\n", ncalcs);
  PRINT_R0("   Average APSP time    : %f sec.\n", average_time);
  PRINT_R0("   Estimated elapse time: %f sec.\n", average_time * ncalcs);
  PRINT_R0("Input filename: %s\n", infname);
  PRINT_R0("   (w x h, d, r) = (%d x %d, %d, %d)\n", width, height, degree, low_length);
  if(enable_outfname)
    PRINT_R0("Output filename: %s\n", outfname);
  PRINT_R0("---\n");
}

static void output_file(FILE *fp, const int lines, const int height, int edge[lines][2])
{
  for(int i=0;i<lines;i++)
    fprintf(fp, "%d,%d %d,%d\n", WIDTH(edge[i][0], height), HEIGHT(edge[i][0], height),
	    WIDTH(edge[i][1], height), HEIGHT(edge[i][1], height));
}

static void check_length(const int lines, const int height, const int length, int edge[lines][2])
{
  for(int i=0;i<lines;i++){
    int w0 = WIDTH (edge[i][0], height);
    int h0 = HEIGHT(edge[i][0], height);
    int w1 = WIDTH (edge[i][1], height);
    int h1 = HEIGHT(edge[i][1], height);
    int distance = abs(w0 - w1) + abs(h0 - h1);
    if(distance > length)
      printf("Over length in line %d: %d,%d %d,%d : length = %d, distance = %d\n",
	     i+1, w0, h0, w1, h1, length, distance);
  }
}

int main(int argc, char *argv[])
{
  bool enable_max_temp = false, enable_min_temp = false, enable_outfname = false;
  bool enable_hill_climbing = false, enable_detect_temp = false, enable_bfs = false;
  bool enable_halfway = false, enable_fixed_temp = false, enable_infname = false;
  char hostname[MPI_MAX_PROCESSOR_NAME], infname[MAX_FILENAME_LENGTH], outfname[MAX_FILENAME_LENGTH];
  int namelen, diam = 0, low_diam = 0, random_seed = 0, cooling_cycle = 1;
  int based_width = 0, based_height = 0, width = 0, height = 0, degree = 0;
  int length = -1, low_length = NOT_DEFINED, groups = 1;
  long long ncalcs = 10000, num_accepts = 0;
  double ASPL = 0, low_ASPL = 0, cooling_rate = 0;
  double max_temp = 100.0, min_temp = 0.217147, fixed_temp = 0, max_diff_energy = 0;
  FILE *fp = NULL;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &procs);
  MPI_Get_processor_name(hostname, &namelen);
  PRINT_R0("Run on %s\n", hostname);
  time_t t = time(NULL);
  PRINT_R0("%s---\n", ctime(&t));

  // Set arguments
  set_args(argc, argv, infname, &low_length, outfname, &enable_outfname, &random_seed,
	   &ncalcs, &max_temp, &enable_max_temp, &min_temp, &enable_min_temp, &cooling_cycle,
	   &enable_hill_climbing, &enable_detect_temp, &enable_bfs, &enable_halfway,
	   &fixed_temp, &enable_fixed_temp, &enable_infname, &width, &height, &degree);

  if(low_length == NOT_DEFINED)
    ERROR("Must need -r\n");
  else if(enable_hill_climbing && enable_max_temp)
    ERROR("Both -Y and -w cannot be used.\n");
  else if(enable_hill_climbing && enable_min_temp)
    ERROR("Both -Y and -c cannot be used.\n");
  else if(enable_hill_climbing && enable_detect_temp)
    ERROR("Both -Y and -d cannot be used.\n");
  else if(max_temp == min_temp)
    ERROR("The same values in -w and -c.\n");
  else if(enable_fixed_temp && min_temp > fixed_temp)
    ERROR("The value in -F (%f) must be less than min_temp in -c (%f)\n",
	  fixed_temp, min_temp);
  else if(!enable_infname && (width == 0 || height == 0 || degree == 0))
    ERROR("Set -W, -H, and -D\n");

  if(enable_detect_temp)
    ncalcs = DEFAULT_NCALCS;

  srandom(random_seed);
  int based_lines = (enable_infname)? count_lines(infname) : (width * height * degree) / 2;
  int lines       = based_lines;
  int (*edge)[2]  = malloc(sizeof(int)*lines*2); // int edge[lines][2];
  int based_nodes, nodes;
  if(enable_infname){
    read_file_lattice(edge, &based_width, &based_height, infname);
    height = based_height;
    width  = based_width;
    based_nodes = max_node_num(based_lines, (int *)edge) + 1;
    nodes  = based_nodes;
    degree = 2 * lines / nodes;
  }
  else{
    create_lattice(lines, (int *)edge, width, height, degree, low_length, random_seed, ncalcs);
    based_nodes  = nodes = width  *height;
    based_height = height;
    based_width  = width;
  }

  // For some reason, if there is no code below, it may end in error.
  int n[nodes];
  for(int i=0;i<nodes;i++)
    n[i] = 0;

  for(int i=0;i<lines;i++){
    n[edge[i][0]]++;
    n[edge[i][1]]++;
  }
  //

  // check distance
  for(int i=0;i<lines;i++){
      int w0 = WIDTH (edge[i][0], height);
      int h0 = HEIGHT(edge[i][0], height);
      int w1 = WIDTH (edge[i][1], height);
      int h1 = HEIGHT(edge[i][1], height);
      int tmp_length = abs(w0 - w1) + abs(h0 - h1);
      if(tmp_length > low_length)
	ERROR("Distance is over\n");
  }

  if(nodes <= degree)
    ERROR("n is too small. nodes = %d degree = %d\n", nodes, degree);
  else if(based_width*based_height != based_nodes)
    ERROR("Not grid graph (width %d x height %d != nodes %d).\n", based_width, based_height, based_nodes);

  int *rotate_hash = malloc(nodes * sizeof(int));
  create_rotate_hash(nodes, height, width, groups, rotate_hash);

  lower_bound_of_diam_aspl(&low_diam, &low_ASPL, width, height, degree, low_length);
  check_current_edge(nodes, lines, edge, low_ASPL, groups, height, based_height, enable_bfs, rotate_hash);
  double average_time = estimated_elapse_time(nodes, lines, (const int (*)[2])edge,
					      height, width, based_height, groups,
					      low_length, enable_bfs, rotate_hash);
  if(enable_hill_climbing){
    fixed_temp = max_temp = min_temp = 0.0;
    cooling_rate = 1.0;
  }
  else{
    cooling_rate = (max_temp != min_temp)? pow(min_temp/max_temp, (double)cooling_cycle/ncalcs) : 1.0;
  }

  if(enable_outfname && rank == 0){
    struct stat stat_buf;
    if(stat(outfname, &stat_buf) == 0)
      ERROR("Output file %s exsits. \n", outfname);
    
    if((fp = fopen(outfname, "w")) == NULL)
      ERROR("Cannot open %s\n", outfname);
  }

  output_params(degree, low_length, random_seed, max_temp, min_temp, ncalcs, cooling_cycle,
		cooling_rate, infname, outfname, enable_outfname, average_time, enable_hill_climbing,
		width, height, enable_bfs, enable_fixed_temp, fixed_temp);

  // Optimization
  timer_clear_all();
  timer_start(TIMER_SA);
  long long step = sa(nodes, lines, max_temp, ncalcs,
		      cooling_rate, low_diam, low_ASPL, enable_bfs, 
		      enable_hill_climbing, enable_detect_temp,
		      &max_diff_energy, max_temp, min_temp,
		      enable_fixed_temp, fixed_temp, edge,
		      &diam, &ASPL, cooling_cycle, &num_accepts, width,
		      based_width, height, based_height, &length,
		      low_length, groups, rotate_hash);
  timer_stop(TIMER_SA);
  
  if(enable_detect_temp){
    // Set max temperature to accept it   50% in maximum diff energy.
    PRINT_R0("Proposed max temperature is %f\n", (-1.0 * max_diff_energy) / log(0.5));
    // Set min temperature to accept it 0.01% in minimum diff energy.
    END("Proposed min temperature is %f\n", (-2.0) / log(0.0001));
  }

  // Output results
  PRINT_R0("---\n");
  PRINT_R0("Diam. k = %d  ASPL l = %f  Diam. gap = %d  ASPL gap = %f Length Gap = %d\n",
	   diam, ASPL, diam-low_diam, ASPL-low_ASPL, length-low_length);

  double time_sa    = timer_read(TIMER_SA);
  double time_apsp  = timer_read(TIMER_APSP);
  double time_check = timer_read(TIMER_CHECK);
  PRINT_R0("Steps: %lld  Elapse time: %f sec. (APSP: %f sec. Check: %f sec. Other: %f sec.)\n",
	   step, time_sa, time_apsp, time_check, time_sa-(time_apsp+time_check));
  if(ncalcs > SKIP_ACCEPTS)
    PRINT_R0("Accept rate: %f (= %lld/%lld)\n",
	     (double)num_accepts/(ncalcs-SKIP_ACCEPTS), num_accepts, ncalcs-SKIP_ACCEPTS);
  if(rank == 0 && enable_outfname){
    output_file(fp, lines, height, edge);
    fclose(fp);
  }

  check_length(lines, height, low_length, edge);

  MPI_Finalize();
  return 0;
}