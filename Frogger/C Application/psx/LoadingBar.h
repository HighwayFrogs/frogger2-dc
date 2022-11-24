#ifndef LOADINGBAR_H_INCLUDE
#define LOADINGBAR_H_INCLUDE

extern int barProgress;

void StepProgressBar ( int percentAmount );

void SetProgressBar ( int percentage );

void DrawLoadingBar ( int execute );


#endif
