#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"

using namespace std;

#include "rtdsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;
  int inNum;
  int unroll_argc = argc - 3;
    
  for (inNum = 2; inNum < unroll_argc; inNum = inNum + 4) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);
    double sample = 0;
      
    if ( ok ) {
      sample = applyFilter(filter, input, output);
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
      
    inputFilename = argv[inNum + 1];
    outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    input = new struct cs1300bmp;
    output = new struct cs1300bmp;
    ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sample += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
    
    inputFilename = argv[inNum + 2];
    outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    input = new struct cs1300bmp;
    output = new struct cs1300bmp;
    ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sample += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
      
    inputFilename = argv[inNum + 3];
    outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    input = new struct cs1300bmp;
    output = new struct cs1300bmp;
    ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sample += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    sum += sample;
    delete input;
    delete output;
      
  }
    
  for (; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
    
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    int j;
    int unroll_size = size-1;

    for (int i=0; i < size; i++) {
      for (j=0; j < unroll_size; j = j + 2) {
          int value;
          input >> value;
          filter -> set(i,j,value);

          int value1;
          input >> value1;
          filter -> set(i,j + 1,value1);
      }

      for (; j < size; j++) {
          int value;
          input >> value;
          filter -> set(i,j,value);
      }
    }
    return filter;
  }
}


double
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output)
{
    
    long long cycStart, cycStop;
    int filter_size = filter -> getSize();
    int unroll_size = filter_size - 9;
    int filter_divisor =  filter -> getDivisor();
    
    cycStart = rdtscll();
    
    output -> width = input -> width;
    output -> height = input -> height;
    
    
    for(int col = 1; col < (input -> width) - 1; col = col + 1) {
        for(int row = 1; row < (input -> height) - 1 ; row = row + 1) {
            
            int j; // outter loop index
            int i; // inner loop index
            int t = 0;
            int output_tempo = 0;
            int output_tempo1 = 0;
            int output_tempo2 = 0;
            int output_tempo3 = 0;
            int output_tempo4 = 0;
            int output_tempo5 = 0;
            int output_tempo6 = 0;
            int output_tempo7 = 0;
            int output_tempo8 = 0;
            int output_tempo9 = 0;
            
            for (j = 0; j < filter_size; j++) {
                for (i = 0; i < unroll_size; i = i + 10) {
                    output_tempo = output_tempo
                    + (input -> color[0][row + i - 1][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo1 = output_tempo1
                    + (input -> color[0][row + i][col + j - 1]
                       * filter -> get(i+1, j) );
                    output_tempo2 = output_tempo2
                    + (input -> color[0][row + i + 1][col + j - 1]
                       * filter -> get(i+2, j) );
                    output_tempo3 = output_tempo3
                    + (input -> color[0][row + i + 2][col + j - 1]
                       * filter -> get(i+3, j) );
                    output_tempo4 = output_tempo4
                    + (input -> color[0][row + i + 3][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo5 = output_tempo5
                    + (input -> color[0][row + i + 4][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo6 = output_tempo6
                    + (input -> color[0][row + i + 5][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo7 = output_tempo7
                    + (input -> color[0][row + i + 6][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo8 = output_tempo8
                    + (input -> color[0][row + i + 7][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo9 = output_tempo9
                    + (input -> color[0][row + i + 8][col + j - 1]
                       * filter -> get(i, j) );
                }
                // deal with the rest elements
                for (; i < filter_size; i++) {
                    output_tempo = output_tempo
                    + (input -> color[0][row + i - 1][col + j - 1]
                       * filter -> get(i, j) );
                }
            }
            
            // combine the output_tempos' together
            output_tempo = output_tempo + output_tempo1
            + output_tempo2 + output_tempo3
            + output_tempo4 + output_tempo5
            + output_tempo6 + output_tempo7
            + output_tempo8 + output_tempo9;
            
            output_tempo = output_tempo / filter_divisor;
            
            if ( output_tempo  < 0 ) {
                output_tempo = 0;
            }
            
            if ( output_tempo > 255 ) {
                output_tempo = 255;
            }
            
            output -> color[0][row][col] = output_tempo;
            
            
        }
    }
    
    for(int col = 1; col < (input -> width) - 1; col = col + 1) {
        for(int row = 1; row < (input -> height) - 1 ; row = row + 1) {
            
            int j; // outter loop index
            int i; // inner loop index
            int t = 0;
            int output_tempo = 0;
            int output_tempo1 = 0;
            int output_tempo2 = 0;
            int output_tempo3 = 0;
            int output_tempo4 = 0;
            int output_tempo5 = 0;
            int output_tempo6 = 0;
            int output_tempo7 = 0;
            int output_tempo8 = 0;
            int output_tempo9 = 0;
            for (j = 0; j < filter_size; j++) {
                for (i = 0; i < unroll_size; i = i + 10) {
                    output_tempo = output_tempo
                    + (input -> color[1][row + i - 1][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo1 = output_tempo1
                    + (input -> color[1][row + i][col + j - 1]
                       * filter -> get(i+1, j) );
                    output_tempo2 = output_tempo2
                    + (input -> color[1][row + i + 1][col + j - 1]
                       * filter -> get(i+2, j) );
                    output_tempo3 = output_tempo3
                    + (input -> color[1][row + i + 2][col + j - 1]
                       * filter -> get(i+3, j) );
                    output_tempo4 = output_tempo4
                    + (input -> color[1][row + i + 3][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo5 = output_tempo5
                    + (input -> color[1][row + i + 4][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo6 = output_tempo6
                    + (input -> color[1][row + i + 5][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo7 = output_tempo7
                    + (input -> color[1][row + i + 6][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo8 = output_tempo8
                    + (input -> color[1][row + i + 7][col + j - 1]
                       * filter -> get(i, j) );
                    output_tempo9 = output_tempo9
                    + (input -> color[1][row + i + 8][col + j - 1]
                       * filter -> get(i, j) );
                    
                }
                // deal with the rest elements
                for (; i < filter_size; i++) {
                    output_tempo = output_tempo
                    + (input -> color[1][row + i - 1][col + j - 1]
                       * filter -> get(i, j) );
                }
            }
            
            // combine the output_tempos' together
            output_tempo = output_tempo + output_tempo1
            + output_tempo2 + output_tempo3
            + output_tempo4 + output_tempo5
            + output_tempo6 + output_tempo7
            + output_tempo8 + output_tempo9;
            
            output_tempo = output_tempo / filter_divisor;
            
            if ( output_tempo  < 0 ) {
                output_tempo = 0;
            }
            
            if ( output_tempo > 255 ) {
                output_tempo = 255;
            }
            
            output -> color[1][row][col] = output_tempo;
        }
    }
    
    for(int col = 1; col < (input -> width) - 1; col = col + 1) {
        for(int row = 1; row < (input -> height) - 1 ; row = row + 1) {
            
            int j; // outter loop index
            int i; // inner loop index
            int t = 0;
            int output_tempo = 0;
            int output_tempo1 = 0;
            int output_tempo2 = 0;
            int output_tempo3 = 0;
            int output_tempo4 = 0;
            int output_tempo5 = 0;
            int output_tempo6 = 0;
            int output_tempo7 = 0;
            int output_tempo8 = 0;
            int output_tempo9 = 0;
            for (j = 0; j < filter_size; j++) {
                for (i = 0; i < unroll_size; i = i + 10) {	
                    output_tempo = output_tempo
                    + (input -> color[2][row + i - 1][col + j - 1] 
                       * filter -> get(i, j) );
                    output_tempo1 = output_tempo1
                    + (input -> color[2][row + i][col + j - 1] 
                       * filter -> get(i+1, j) );
                    output_tempo2 = output_tempo2
                    + (input -> color[2][row + i + 1][col + j - 1] 
                       * filter -> get(i+2, j) );
                    output_tempo3 = output_tempo3
                    + (input -> color[2][row + i + 2][col + j - 1] 
                       * filter -> get(i+3, j) );
                    output_tempo4 = output_tempo4
                    + (input -> color[2][row + i + 3][col + j - 1] 
                       * filter -> get(i, j) );
                    output_tempo5 = output_tempo5
                    + (input -> color[2][row + i + 4][col + j - 1] 
                       * filter -> get(i, j) );
                    output_tempo6 = output_tempo6
                    + (input -> color[2][row + i + 5][col + j - 1] 
                       * filter -> get(i, j) );
                    output_tempo7 = output_tempo7
                    + (input -> color[2][row + i + 6][col + j - 1] 
                       * filter -> get(i, j) );
                    output_tempo8 = output_tempo8
                    + (input -> color[2][row + i + 7][col + j - 1] 
                       * filter -> get(i, j) );
                    output_tempo9 = output_tempo9
                    + (input -> color[2][row + i + 8][col + j - 1] 
                       * filter -> get(i, j) );
                }
                // deal with the rest elements
                for (; i < filter_size; i++) {
                    output_tempo = output_tempo
                    + (input -> color[2][row + i - 1][col + j - 1] 
                       * filter -> get(i, j) );
                }
            }
            
            // combine the output_tempos' together
            output_tempo = output_tempo + output_tempo1
            + output_tempo2 + output_tempo3
            + output_tempo4 + output_tempo5
            + output_tempo6 + output_tempo7
            + output_tempo8 + output_tempo9;
            
            output_tempo = output_tempo / filter_divisor;
            
            if ( output_tempo  < 0 ) {
                output_tempo = 0;
            }
            
            if ( output_tempo > 255 ) { 
                output_tempo = 255;
            }
            
            output -> color[2][row][col] = output_tempo;
        }
  }  

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
