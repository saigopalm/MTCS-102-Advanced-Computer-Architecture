/* Twobit branch predictor simulator
   Author: Mantha Sai Gopal
   Reg.no: 23358
*/


#define __STDC_FORMAT_MACROS
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define SIZE 157

char string[SIZE];
int str_len = 0;

void simulate(FILE* inputFile, FILE* outputFile)
{
  // See the documentation to understand what these variables mean.
  int32_t microOpCount;
  uint64_t instructionAddress;
  int32_t sourceRegister1;
  int32_t sourceRegister2;
  int32_t destinationRegister;
  char conditionRegister;
  char TNnotBranch;
  char loadStore;
  int64_t immediate;
  uint64_t addressForMemoryOp;
  uint64_t fallthroughPC;
  uint64_t targetAddressTakenBranch;
  char macroOperation[12];
  char microOperation[23];

  int64_t totalMicroops = 0;
  int64_t totalMacroops = 0;

  int aluCount = 0;
  int loadStoreCount = 0;
  int unconditionalBranchCount = 0;
  int conditionalBranchCount = 0;
  int takenCount = 0;
  int notTakenCount = 0;
  int fpAddCount = 0;
  int fpDivCount = 0;
  int fpMulCount = 0;
  int otherFpCount = 0;
  float averCpi = 0;
  char ch;

  
  fprintf(outputFile, "Processing trace...\n");
  
  while (true) {
    int result = fscanf(inputFile, 
                        "%" SCNi32
                        "%" SCNx64 
                        "%" SCNi32
                        "%" SCNi32
                        "%" SCNi32
                        " %c"
                        " %c"
                        " %c"
                        "%" SCNi64
                        "%" SCNx64
                        "%" SCNx64
                        "%" SCNx64
                        "%11s"
                        "%22s",
                        &microOpCount,
                        &instructionAddress,
                        &sourceRegister1,
                        &sourceRegister2,
                        &destinationRegister,
                        &conditionRegister,
                        &TNnotBranch,
                        &loadStore,
                        &immediate,
                        &addressForMemoryOp,
                        &fallthroughPC,
                        &targetAddressTakenBranch,
                        macroOperation,
                        microOperation);
                        
    if (result == EOF) {
      break;
    }

    if (result != 14) {
      fprintf(stderr, "Error parsing trace at line %" PRIi64 "\n", totalMicroops);
      abort();
    }

    // For each micro-op
    totalMicroops++;

    // For counting ALU instructions
    if (strncmp("ADD", microOperation,3) == 0 || strncmp("SUB", microOperation,3) == 0 || strncmp("MUL", microOperation,3) == 0 || 
    strncmp("DIV", microOperation,3) == 0 || strncmp("SHR", microOperation,3) == 0 || strncmp("SHL", microOperation,3) == 0 || strncmp("AND", microOperation,3) == 0 || strncmp("OR", microOperation,2) == 0 || strncmp("XOR", microOperation,3) == 0 || strncmp("NOT", microOperation,3) == 0)
    {
      aluCount++;
    }
    
    // For counting load operations
    if(loadStore == 'L' || loadStore == 'S') {
        loadStoreCount++;
    }

    // For counting number of unconditional branches
    if(targetAddressTakenBranch != 0 && conditionRegister == '-'){
      unconditionalBranchCount++;
    }

    // For counting number of conditional branches
    if(targetAddressTakenBranch != 0 && conditionRegister == 'R') {
      conditionalBranchCount++;

      if(TNnotBranch == 'T') {
        //fprintf(outputFile, "T");
        ch = 'T';
        strncat(string, &ch, 1);
        str_len++;
        takenCount++;
      }
      else if(TNnotBranch == 'N') {
        //fprintf(outputFile, "N");
        ch = 'N';
        strncat(string, &ch, 1);
        str_len++;
        notTakenCount++;
      }
    }

    // For floating operations

    if (strncmp("FP_ADD", microOperation,6) == 0)
    {
      fpAddCount++;
    }
    else if (strncmp("FP_MUL", microOperation, 6) == 0) {
      fpMulCount++;
    }
    else if (strncmp("FP_DIV", microOperation, 6) == 0) {
      fpDivCount++;
    }
    else if (strncmp("FP", microOperation, 2) == 0) {
      otherFpCount++;
    }

    // For each macro-op:
    if (microOpCount == 1) {
      totalMacroops++;
    }

    // For calulating average cpi 
    averCpi = ((aluCount * 1 + loadStoreCount * 4 + unconditionalBranchCount * 2 + conditionalBranchCount * 4 + fpAddCount * 3 + fpDivCount * 30 + fpMulCount * 3) / (fpAddCount + fpDivCount + fpMulCount + aluCount + conditionalBranchCount + unconditionalBranchCount + loadStoreCount));
  }
  
  fprintf(outputFile, "Processed %" PRIi64 " trace records.\n\n", totalMicroops);

  fprintf(outputFile, "Micro-ops: %" PRIi64 "\n", totalMicroops);
  fprintf(outputFile, "Macro-ops: %" PRIi64 "\n\n", totalMacroops);
  fprintf(outputFile, "ALU Instructions: %d (%f %%) \n\n", aluCount, (float)aluCount*100/totalMicroops);
  fprintf(outputFile, "Load-Store Instructions: %d (%f %%) \n\n", loadStoreCount, (float)loadStoreCount*100/totalMicroops);
  fprintf(outputFile, "Unconditional Branches: %d (%f %%) \n\n", unconditionalBranchCount, (float)unconditionalBranchCount*100/totalMicroops);
  fprintf(outputFile, "Conditional Branches: %d (%f %%) \n", conditionalBranchCount, (float)conditionalBranchCount*100/totalMicroops);
  fprintf(outputFile, " Taken: %d \n", takenCount);
  fprintf(outputFile, " Not Taken: %d \n\n", notTakenCount);
  fprintf(outputFile, "Float Point Instructions: %d (%f %%) \n",(fpAddCount + fpDivCount + fpMulCount), (float)(fpAddCount + fpDivCount + fpMulCount + otherFpCount)*100/totalMicroops);
  fprintf(outputFile, " FP Add: %d \n", fpAddCount);
  fprintf(outputFile, " FP MUL: %d \n", fpMulCount);
  fprintf(outputFile, " FP DIV: %d \n", fpDivCount);
  fprintf(outputFile, " Other FP: %d \n", otherFpCount);
  int otherOpsCount = totalMicroops - aluCount - loadStoreCount - unconditionalBranchCount - conditionalBranchCount - fpAddCount - fpDivCount - fpMulCount - otherFpCount;
  fprintf(outputFile, "Other Instructions: %d (%f %%) \n", otherOpsCount, (float)otherOpsCount*100 / totalMicroops);
  fprintf(outputFile, " Average CPI: %f \n", averCpi);


}

void TwobitPredictor(){
  // Assume that initially branch is strongly taken
  char prediction_bit = 'T';
  char hysteresis_bit = 'T';
  
  // variable to keep track of number of correct(and mis)predictions
  int correct_predictions = 0;
  int mispredictions = 0;
  float misprediction_rate = 0.0;
  
  // Simulate the branch prediction for 'str_len' iterations
  for (int i = 0; i < str_len; i++) {
    if(string[i] == 'T'){
      if(hysteresis_bit = 'T' && prediction_bit == 'T'){
        correct_predictions++;
      }
      else if(prediction_bit == 'T' && hysteresis_bit == 'N'){
        hysteresis_bit = 'T';
        correct_predictions++;
      }
      else if(prediction_bit == 'N' && hysteresis_bit == 'T'){
        hysteresis_bit = 'N';
        prediction_bit = 'T';
        mispredictions++;
      }
      else{
        mispredictions++;
        hysteresis_bit = 'T';
      }
    }
    else{
      if(hysteresis_bit == 'T' && prediction_bit == 'T'){
        hysteresis_bit = 'N';
        mispredictions++;
      }
      else if(hysteresis_bit == 'N' && prediction_bit == 'T'){
        mispredictions++;
        hysteresis_bit = 'T';
        prediction_bit = 'N';
      }
      else if(hysteresis_bit == 'T' && prediction_bit == 'N'){
        hysteresis_bit = 'N';
        correct_predictions++;
      }
      else{
        correct_predictions++;
      }
    }
  }

  printf("Total Branches: %d", correct_predictions + mispredictions);
  printf("\nCorrect Predictions: %d", correct_predictions);
  printf("\nMispredictions: %d", mispredictions);
  misprediction_rate = (float) mispredictions / (str_len) * 100; 
  printf("\nMisprediction rate: %.2f%%", misprediction_rate);
  printf("\nAccuracy: %.2f%%\n", 100 - misprediction_rate);

}

int main(int argc, char *argv[]) 
{
  FILE *inputFile = stdin;
  FILE *outputFile = stdout;
  
  if (argc >= 2) {
    inputFile = fopen(argv[1], "r");
    assert(inputFile != NULL);
  }
  if (argc >= 3) {
    outputFile = fopen(argv[2], "w");
    assert(outputFile != NULL);
  }
  
  simulate(inputFile, outputFile);

  TwobitPredictor();
  return 0;
}
