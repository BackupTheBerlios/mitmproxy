
int loadFile(const char *path)
{
  lst = stderr;
  /* check on correct parameter usage */
  strcpy(SourceName, path);
/*
  param_options(argc, argv);
  if (!SourceName[0]) {
    fprintf(stderr, "No input file specified\n");
    exit(EXIT_FAILURE);
  }
*/
  /* open the source file (Scanner.S_src)  */
  if ((S_src = open(SourceName, O_RDONLY|O_BINARY)) == -1) {
    fprintf(stderr, "Unable to open input file %s\n", SourceName);
    exit(EXIT_FAILURE);
  }

  /* install error reporting procedure */
  Custom_Error = (Error_Func) StoreError;

  /* parse the source */
  Parse();
  close(S_src);

  if (Errors) fprintf(stderr, "Compilation errors\n");

  if (Listinfo) SourceListing();
  else if (Errors) SummarizeErrors();

  return(Errors);
}


